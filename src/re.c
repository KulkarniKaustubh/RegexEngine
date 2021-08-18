#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re.h"

int isLowercaseAlphabet(char ch) { if (ch >= 'a' && ch <= 'z') return 1; return 0; }

int isUppercaseAlphabet(char ch) { if (ch >= 'A' && ch <= 'Z') return 1; return 0; }

int isDigit(char ch) { if (ch >= '0' && ch <= '9') return 1; return 0; }

int isUnderscore(char ch) { if (ch == '_') return 1; return 0; }

int isInRange(char ch, char ch1, char ch2) { if (ch >= ch1 && ch <= ch2) return 1; return 0; }

int isValidEscapeCharacter(char escape_ch, char text_ch)
{
	if ((escape_ch == 's' && text_ch == ' ') ||
		(escape_ch == 'w' && (isLowercaseAlphabet(text_ch) ||
							isUppercaseAlphabet(text_ch) ||
							isDigit(text_ch) ||
							isUnderscore(text_ch))) ||
		(escape_ch == 'd' && isDigit(text_ch)) ||
		(escape_ch == '\\' && text_ch == '\\') ||
		(escape_ch == '.' && text_ch == '.') ||
		(escape_ch == '*' && text_ch == '*') ||
		(escape_ch == '+' && text_ch == '+') ||
		(escape_ch == '?' && text_ch == '?') ||
		(escape_ch == '[' && text_ch == '[') ||
		(escape_ch == ']' && text_ch == ']')) return 1;

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
		int go_ahead = 0;
		do {
			index_arr = match_letter(pat, text, index_arr);
			if (index_arr[0] == 0) {
				go_ahead++;
				text++;
				index_arr[1] = go_ahead;
				index_arr[2] = go_ahead;
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

	if (*pat == '\0' || (*pat == '$' && *text == '\0')) {
		index_arr[0] = 1;
		return index_arr;
	}

	int escape_flag = 0; int star_or_plus = 0; int greedy = 1;

	if (*pat == '[') {
		char *p;

		for (p = pat+1; *p != ']' && *p != '\0'; ++p);
		p++;

		int go_ahead = 0;

		if (p[0] == '*') {
			if (p[1] == '?') {
				index_arr = match_character_class_star_plus(pat + 1, p, text, index_arr, &go_ahead);
				return match_letter(p + 2, text + go_ahead, index_arr);
			} else {
				index_arr = match_character_class_star_plus_greedy(pat + 1, p, text, index_arr, &go_ahead);
				return match_letter(p + 1, text + go_ahead, index_arr);
			}
		} else if (p[0] == '+') {
			if (p[1] == '?') {
				char *ptr = pat + 1;

				while (*ptr != ']') {
					if (*text != '\0' && (*text == *ptr || *ptr == '.')) {
						index_arr[2] += 1;
						text++;
						index_arr = match_character_class_star_plus(pat + 1, p, text, index_arr, &go_ahead);
						return match_letter(p + 2, text + go_ahead, index_arr);
					} else if (*text != '\0' && ptr[1] == '-' && *text >= ptr[0] && *text <= ptr[2]) {
						// int temp_arr[3] = {0, 0, 0};
						// int *temp_match = match_letter(p + 2, text + 1, temp_arr);
						//
						// if (temp_match[0] == 0) break;

						index_arr[2] += 1;
						text++;
						index_arr = match_character_class_star_plus(pat + 1, p, text, index_arr, &go_ahead);
						return match_letter(p + 1, text + go_ahead, index_arr);
					} else if (*text != '\0' && ptr[1] == '-' && (*text < ptr[0] || *text > ptr[2])) {
						ptr += 3;
					} else {
						ptr += 1;
					}
				}
			} else {
				char *ptr = pat + 1;

				while (*ptr != ']') {
					if (*text != '\0' && (*text == *ptr || *ptr == '.')) {
						// int temp_arr[3] = {0, 0, 0};
						// int *temp_match = match_letter(p + 1, text + 1, temp_arr);
						//
						// if (temp_match[0] == 0) break;

						index_arr[2] += 1;
						text++;
						index_arr = match_character_class_star_plus_greedy(pat + 1, p, text, index_arr, &go_ahead);
						return match_letter(p + 1, text + go_ahead, index_arr);
					} else if (*text != '\0' && ptr[1] == '-' && *text >= ptr[0] && *text <= ptr[2]) {
						// int temp_arr[3] = {0, 0, 0};
						// int *temp_match = match_letter(p + 1, text + 1, temp_arr);
						//
						// if (temp_match[0] == 0) break;

						index_arr[2] += 1;
						text++;
						index_arr = match_character_class_star_plus_greedy(pat + 1, p, text, index_arr, &go_ahead);
						return match_letter(p + 1, text + go_ahead, index_arr);
					} else if (*text != '\0' && ptr[1] == '-' && (*text < ptr[0] || *text > ptr[2])) {
						ptr += 3;
					} else {
						ptr += 1;
					}
				}

			}
		} else if (p[0] == '?') {
			pat += 1;

			while (*pat != ']') {
				if (*text != '\0' && (*text == *pat || *pat == '.')) {
					int temp_arr[3] = {0, 0, 0};
					int *temp_match = match_letter(p + 1, text + 1, temp_arr);

					if (temp_match[0] == 0) break;

					index_arr[2] += 1;
					text++;
					break;
				} else if (*text != '\0' && pat[1] == '-' && *text >= pat[0] && *text <= pat[2]) {
					int temp_arr[3] = {0, 0, 0};
					int *temp_match = match_letter(p + 1, text + 1, temp_arr);

					if (temp_match[0] == 0) break;

					index_arr[2] += 1;
					text++;
					break;
				} else if (*text != '\0' && pat[1] == '-' && (*text < pat[0] || *text > pat[2])) {
					pat += 3;
				} else {
					pat += 1;
				}
			}

			return match_letter(p + 1, text, index_arr);
		} else {
			pat += 1;

			while (*pat != ']') {
				if (*text != '\0' && (*text == *pat || *pat == '.')) {
					int temp_arr[3] = {0, 0, 0};
					int *temp_match = match_letter(p, text + 1, temp_arr);

					if (temp_match[0] == 0) break;

					index_arr[2] += 1;
					return match_letter(p, text + 1, index_arr);
				} else if (*text != '\0' && pat[1] == '-' && *text >= pat[0] && *text <= pat[2]) {
					int temp_arr[3] = {0, 0, 0};
					int *temp_match = match_letter(p, text + 1, temp_arr);

					if (temp_match[0] == 0) break;

					index_arr[2] += 1;
					return match_letter(p, text + 1, index_arr);
				} else if (*text != '\0' && pat[1] == '-' && (*text < pat[0] || *text > pat[2])) {
					pat += 3;
				} else {
					pat += 1;
				}
			}
		}

		return index_arr;
	}

	if (*pat == '\\') {
		pat++;
		escape_flag = 1;
	}

	if (pat[1] == '*') {
		if (pat[2] == '?') greedy = 0;
		star_or_plus = 1;
	} else if (pat[1] == '+') {
		if (pat[2] == '?') greedy = 0;
		if (escape_flag && isValidEscapeCharacter(*pat, *text)) {
			star_or_plus = 1;
			text++;
			index_arr[2] += 1;
		}
		else if (!escape_flag && *text != '\0' && (*pat == '.' || *pat == *text)) {
			star_or_plus = 1;
			text++;
			index_arr[2] += 1;
		}
	} else if (pat[1] == '?') {
		if (!escape_flag && *text != '\0' && (*pat == '.' || *pat == *text)) {
			int temp_arr[3] = {0, 0, 0};
			int *temp_match = match_letter(pat + 2, text + 1, temp_arr);

			if (temp_match[0] == 0) {
				return match_letter(pat + 2, text, index_arr);
			} else {
				index_arr[2] += 1;
				return match_letter(pat + 2, text + 1, index_arr);
			}
		} else if (escape_flag && isValidEscapeCharacter(*pat, *text)) {
			int temp_arr[3] = {0, 0, 0};
			int *temp_match = match_letter(pat + 2, text + 1, temp_arr);

			if (temp_match[0] == 0) {
				return match_letter(pat + 2, text, index_arr);
			} else {
				index_arr[2] += 1;
				return match_letter(pat + 2, text + 1, index_arr);
			}
		} else {
			return match_letter(pat + 2, text, index_arr);
		}
	}

	if (escape_flag && !star_or_plus && isValidEscapeCharacter(*pat, *text)) {
		index_arr[2] += 1;
		return match_letter(pat + 1, text + 1, index_arr);
	}

	if (star_or_plus) {
		int go_ahead = 0;

		if (!greedy) {
			index_arr = match_star_plus(pat, text, index_arr, escape_flag, &go_ahead);
			return match_letter(pat + 3, text + go_ahead, index_arr);
		}

		if (greedy) {
			index_arr = match_star_plus_greedy(pat, text, index_arr, escape_flag, &go_ahead);
			return match_letter(pat + 2, text + go_ahead, index_arr);
		}
	}

	if (!escape_flag && *text != '\0' && (*pat == '.' || *pat == *text)) {
		index_arr[2] += 1;
		return match_letter(pat + 1, text + 1, index_arr);
	}

	return index_arr;
}

#if 1
int* match_character_class_star_plus(char *pat, char *p, char *text, int *index_arr, int *go_ahead)
{
	if (*text != '\0') {
		if (p[2] == '\0') return index_arr;

		char *ptr = pat;

		while (*ptr != ']') {
			int temp_arr[3] = {0, 0, 0};
			int *temp_match = match_letter(p + 2, text, temp_arr);

			if (temp_match[0] == 1) break;

			if (*text != '\0' && (*text == *ptr || *ptr == '.')) {
				index_arr[2] += 1;
				(*go_ahead)++;
				ptr = pat;
				text++;
			} else if (*text != '\0' && ptr[1] == '-' && *text >= ptr[0] && *text <= ptr[2]) {
				index_arr[2] += 1;
				(*go_ahead)++;
				ptr = pat;
				text++;
			} else if (*text != '\0' && ptr[1] == '-' && (*text < ptr[0] || *text > ptr[2])) {
				ptr += 3;
			} else {
				ptr += 1;
			}
		}
		return index_arr;
	} else {
		return index_arr;
	}
}
#endif

#if 1
int* match_character_class_star_plus_greedy(char *pat, char *p, char *text, int *index_arr, int *go_ahead)
{
	if (*text != '\0') {
		char *t = text;
		char *ptr = pat;

		while (*ptr != ']') {
			if (*t != '\0' && (*t == *ptr || *ptr == '.')) {
				(*go_ahead)++;
				ptr = pat;
				t++;
			} else if (*t != '\0' && ptr[1] == '-' && *t >= ptr[0] && *t <= ptr[2]) {
				(*go_ahead)++;
				ptr = pat;
				t++;
			} else if (*t != '\0' && ptr[1] == '-' && (*t < ptr[0] || *t > ptr[2])) {
				ptr += 3;
			} else {
				ptr += 1;
			}
		}

		do {
			int temp_arr[3] = {0, 0, 0};
			int *temp_match = match_letter(p + 1, t, temp_arr);
			if (temp_match[0] == 1) break;
			else if (t > text) {
				(*go_ahead)--;
			}
		} while (t-- > text);

		index_arr[2] += *go_ahead;
		return index_arr;
	} else {
		return index_arr;
	}
}
#endif

#if 1
int* match_star_plus_greedy(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead)
{
	if (*text != '\0') {
		if (!escape_flag) {
			char *t;

			for (t = text; *t != '\0' && (*t == *pat || *pat == '.'); ++t, ++(*go_ahead));
			int go_ahead_flag = 0;
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 2, t, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (t > text) {
					(*go_ahead)--;
				}
				// } else if (go_ahead_flag) {
				// 	(*go_ahead)--;
				// }
				go_ahead_flag = 1;
			} while (t-- > text);

			// if (t != text) index_arr[0] = 1;
			index_arr[2] += *go_ahead;
			return index_arr;
		} else {
			char *t;

			for (t = text; *t != '\0' && isValidEscapeCharacter(*pat, *t); ++t, ++(*go_ahead));
			int go_ahead_flag = 0;
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 2, t, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (t > text) {
					(*go_ahead)--;
				}
				// } else if (go_ahead_flag) {
				// 	(*go_ahead)--;
				// }
				go_ahead_flag = 1;
			} while (t-- > text);

			index_arr[2] += *go_ahead;
			return index_arr;
		}
	} else {
		return index_arr;
	}
}
#endif

#if 1
int* match_star_plus(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead)
{
	if (*text != '\0') {
		if (pat[3] == '\0') return index_arr;

		if (!escape_flag) {
			int go_ahead_flag = 0;
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 3, text, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (*text != '\0' && (*text == *pat || *pat == '.')) {
					(*go_ahead)++;
				}
				// } else if (go_ahead_flag) {
				// 	(*go_ahead)++;
				// }
				go_ahead_flag = 1;
			} while (*text != '\0' && (*text++ == *pat || *pat == '.'));

			index_arr[2] += *go_ahead;
			return index_arr;
		} else {
			int go_ahead_flag = 0;
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 3, text, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (*text != '\0' && isValidEscapeCharacter(*pat, *text)) {
					(*go_ahead)++;
				}
				// } else if (go_ahead_flag) {
				// 	(*go_ahead)++;
				// }
				go_ahead_flag = 1;
			} while (*text != '\0' && isValidEscapeCharacter(*pat, *text++));

			index_arr[2] += *go_ahead;
			return index_arr;
		}
	} else {
		return index_arr;
	}
}
#endif
