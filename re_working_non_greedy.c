#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re.h"

int isLowerAlphabet(char ch) { if (ch >= 'a' && ch <= 'z') return 1; return 0; }

int isUpperAlphabet(char ch) { if (ch >= 'A' && ch <= 'Z') return 1; return 0; }

int isDigit(char ch) { if (ch >= '0' && ch <= '9') return 1; return 0; }

int isUnderscore(char ch) { if (ch == '_') return 1; return 0; }

int checkValidEscapeCharacter(char escape_ch, char text_ch)
{
	if ((escape_ch == 's' && text_ch == ' ') ||
		(escape_ch == 'w' && (isLowerAlphabet(text_ch) ||
							isUpperAlphabet(text_ch) ||
							isDigit(text_ch) ||
							isUnderscore(text_ch))) ||
		(escape_ch == 'd' && isDigit(text_ch)) ||
		(escape_ch == '\\' && text_ch == '\\') ||
		(escape_ch == '.' && text_ch == '.') ||
		(escape_ch == '*' && text_ch == '*') ||
		(escape_ch == '+' && text_ch == '+')) return 1;

	return 0;
}

int* match(char *pat, char *text)
{
	int *index_arr = (int*)malloc(sizeof(int)*3);
	for (int i=0; i<3; ++i) {
		index_arr[i] = 0;
	}
	if (*pat == '^') {
		index_arr = match_letter(pat + 1, text, index_arr);
		// for (int i=0; i<3; ++i) printf("%d ", index_arr[i]);
		// printf("\n");
	} else {
		do {
			index_arr = match_letter(pat, text, index_arr);
			if (index_arr[0] == 0) {
				*text++;
				index_arr[1]++;
				index_arr[2]++;
				// if (index_arr[2] > strlen(text)) break;
			}
			else break;
		} while (*text != '\0');
	}
	if (index_arr[2] > 0) index_arr[2]--;
	return index_arr;
}

int* match_letter(char *pat, char *text, int *index_arr)
{
	// printf("----Match Letter-----\n");
	// printf("here pat is : %s and text is : %s\n", pat, text);
	// printf("ascii of *text is : %d\n", (int)*text);
	// printf("-----Match Letter End-----\n");

	if (*pat == '\0') {
		index_arr[0] = 1;
		return index_arr;
	}

	int escape_flag = 0; int star_or_plus = 0; int greedy = 1;

	if (*pat == '\\') {
		*pat++;
		escape_flag = 1;
	}

	if (*(pat+1) == '*') {
		if (*(pat+2) == '?') greedy = 0;
		star_or_plus = 1;
	} else if (*(pat+1) == '+') {
		if (*(pat+2) == '?') greedy = 0;
		if (escape_flag && checkValidEscapeCharacter(*pat, *text)) star_or_plus = 1;
		else if (*text != '\0' && (*pat == '.' || *pat == *text)) star_or_plus = 1;
	} else if (*(pat+1) == '?') {
		if (escape_flag && checkValidEscapeCharacter(*pat, *text)) {
			index_arr[2] += 1;
			return match_letter(pat + 2, text + 1, index_arr);
		} else if (*text != '\0' && (*pat == '.' || *pat == *text)) {
			index_arr[2] += 1;
			return match_letter(pat + 2, text + 1, index_arr);
		} else {
			return match_letter(pat + 2, text, index_arr);
		}
	}

	if (escape_flag) {
		if (star_or_plus && !greedy) {
			int go_ahead = 0;

			index_arr = match_star_plus(pat, text, index_arr, escape_flag, &go_ahead);
			return match_letter(pat + 3, text + go_ahead, index_arr);
		} else if (checkValidEscapeCharacter(*pat, *text)) {
			index_arr[2] += 1;
			return match_letter(pat + 1, text + 1, index_arr);
		}
	}

	if (star_or_plus && !greedy) {
		int go_ahead = 0;

		index_arr = match_star_plus(pat, text, index_arr, escape_flag, &go_ahead);
		return match_letter(pat + 3, text + go_ahead, index_arr);
	}

	if (*text != '\0' && (*pat == '.' || *pat == *text)) {
		index_arr[2] += 1;
		return match_letter(pat + 1, text + 1, index_arr);
	}

	return index_arr;
}

int* match_star_plus(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead)
{
	// printf("-----Star Plus None Greedy-----\n");
	// printf("here pat is : %s and text is : %s\n", pat, text);
	// printf("-----Star Plus End Non Greedy-----\n");

	if (!escape_flag) {
		if (*pat == *text && *text != '\0') {
			index_arr[2] += 1;
			(*go_ahead)++;
			return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
		} else if (*pat == '.') {
			if (*(pat+3) == *text && *text != '\0') {
				int *temp_match = match(pat + 3, text);

				if (temp_match && temp_match[0] == 1) {
					if (temp_match[1] == 0) return index_arr;
					else {
						// index_arr[2] += temp_match[1];
						// (*go_ahead) += temp_match[1];
						// return match_star_plus(pat, text + temp_match[1], index_arr, escape_flag, go_ahead);
						index_arr[2] += 1;
						(*go_ahead) ++;
						return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
					}
				} else if (temp_match && temp_match[0] == 0) {
					// index_arr[2] += temp_match[2];
					// (*go_ahead) += temp_match[2];
					index_arr[2] += 1;
					(*go_ahead)++;
					// return match_star_plus(pat, text + temp_match[2], index_arr, escape_flag, go_ahead);
					return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
				}
			} else if (*(pat+3) == '\\' && checkValidEscapeCharacter(*(pat+4), *text)) {
				return index_arr;
			} else if (*(pat+3) == '\0') {
				return index_arr;
			} else {
				index_arr[2] += 1;
				(*go_ahead)++;
				return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
			}
		} else {
			return index_arr;
		}
	} else {
		// printf("*pat, pat+1, pat+2, pat+3 : %c, %c, %c, %d  ", *pat, *(pat+1), *(pat+2), (int)*(pat+3));
		// printf("*text : %c\n", *text);
		// printf("here pat is : %s and text is : %s\n", pat, text);

		if (*(pat+3) == *text && *text != '\0') {
			int *temp_match = match(pat + 3, text);
			// for (int i=0; i<3; ++i) printf("%d ", temp_match[i]);
			// printf("\n");

			if (temp_match && temp_match[0] == 1) {
				if (temp_match[1] == 0) return index_arr;
				else {
					index_arr[2] += 1;
					(*go_ahead) ++;
					return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
				}
			} else if (temp_match && temp_match[0] == 0) {
				index_arr[2] += 1;
				(*go_ahead)++;
				return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
			}
		} else if (*(pat+3) == '\\' && checkValidEscapeCharacter(*(pat+4), *text)) {
			return index_arr;
		} else  if (*(pat+3) == '\0') {
			// printf("Heeeeere\n");
			return index_arr;
		} else if (checkValidEscapeCharacter(*pat, *text) && *text != '\0') {
			int *temp_match = match(pat + 3, text);

			if (temp_match && temp_match[0] == 1) {
				if (temp_match[1] == 0) return index_arr;
				else {
					index_arr[2] += 1;
					(*go_ahead) ++;
					return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
				}
			} else if (temp_match && temp_match[0] == 0) {
				index_arr[2] += 1;
				(*go_ahead)++;
				return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
			}
		} else {
			return index_arr;
		}
	}
}

#if 0
int* match_star_plus_greedy(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead)
{
	if (!escape_flag) {
		if (*pat == *text && *text != '\0') {
			index_arr[2] += 1;
			(*go_ahead)++;
			return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
		} else if (*pat == '.') {
			if (*(pat+3) == *text && *text != '\0') {
				int *temp_match = match(pat + 3, text);

				if (temp_match && temp_match[0] == 1) {
					if (temp_match[1] == 0) return index_arr;
					else {
						index_arr[2] += temp_match[1];
						(*go_ahead) += temp_match[1];
						return match_star_plus(pat, text + temp_match[1], index_arr, escape_flag, go_ahead);
					}
				} else if (temp_match && temp_match[0] == 0) {
					index_arr[2] += temp_match[2];
					(*go_ahead) += temp_match[2];
					return match_star_plus(pat, text + temp_match[2], index_arr, escape_flag, go_ahead);
				}
			} else if (*(pat+3) == '\\' && checkValidEscapeCharacter(*(pat+4), *text)) {
				return index_arr;
			} else if (*(pat+3) == '\0') {
				return index_arr;
			} else {
				index_arr[2] += 1;
				(*go_ahead)++;
				return match_star_plus(pat, text + 1, index_arr, escape_flag, go_ahead);
			}
		} else {
			return index_arr;
		}
	} else {
		if (*(pat+2) == *text) {
			int temp_arr[3] = {0, 0, 0};
			int *temp_match = match_letter(pat + 2, text, temp_arr);
			// for (int i=0; i<3; ++i) printf("%d ", temp_match[i]);
			// printf("\n");

			if (temp_match && temp_match[0] == 1) {
				return index_arr;
			} else if (temp_match && temp_match[0] == 0) {
				index_arr[2] += temp_match[2];
				(*go_ahead) += temp_match[2];
				return match_star_plus_greedy(pat, text + temp_match[2], index_arr, escape_flag, go_ahead);
			}
		} else if (*(pat+2) == '\\' && checkValidEscapeCharacter(*(pat+3), *text)) {
			return index_arr;
		} else if (checkValidEscapeCharacter(*pat, *text)) {
			index_arr[2] += 1;
			(*go_ahead)++;
			return match_star_plus_greedy(pat, text + 1, index_arr, escape_flag, go_ahead);
		} else {
			return index_arr;
		}
	}
}
#endif