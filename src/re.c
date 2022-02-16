#include <stdio.h>
#include <stdlib.h>
#include <string.h> 	// For strlen() while using fgets for the inputs

#define MAX 5000	// Defining the maximum length of text and pattern

// All function definitions
int* match(char *pat, char *text);
int* match_letter(char *pat, char *text, int *index_arr);
int* match_star_plus(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead);
int* match_star_plus_greedy(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead);
int* match_character_class_star_plus(char *pat, char *p, char *text, int *index_arr, int *go_ahead);
int* match_character_class_star_plus_greedy(char *pat, char *p, char *text, int *index_arr, int *go_ahead);

// Function to check for a lower case alphabet
int isLowercaseAlphabet(char ch) { if (ch >= 'a' && ch <= 'z') return 1; return 0; }

// Function to check for an upper case alphabet
int isUppercaseAlphabet(char ch) { if (ch >= 'A' && ch <= 'Z') return 1; return 0; }

// Function to check for a digit
int isDigit(char ch) { if (ch >= '0' && ch <= '9') return 1; return 0; }

// Function to check for an underscore
int isUnderscore(char ch) { if (ch == '_') return 1; return 0; }

// Function to check if a character is in range of two characters for character classes
int isInRange(char ch, char ch1, char ch2) { if (ch >= ch1 && ch <= ch2) return 1; return 0; }

// Function to check if an escaped character and the corresponding text character are valid
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

// Main match function
int* match(char *pat, char *text)
{
	int *index_arr = (int*)malloc(sizeof(int)*3);
	for (int i=0; i<3; ++i) {
		index_arr[i] = 0;
	}
	if (*pat == '^') {
		index_arr = match_letter(pat + 1, text, index_arr); 	// Match from beginning
	} else { 	// Match from every point until text is exhausted
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

// Function to match from beginning
int* match_letter(char *pat, char *text, int *index_arr)
{
	// Check if the full pattern is matched or check if the pattern is at the end of text
	if (*pat == '\0' || (*pat == '$' && *text == '\0')) {
		index_arr[0] = 1;
		return index_arr;
	}

	int escape_flag = 0; int star_or_plus = 0; int greedy = 1;	// Flags to check for an escape character, star or a plus
																// and greedy approach or non-greedy

	// For character classes
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
						index_arr[2] += 1;
						text++;
						index_arr = match_character_class_star_plus_greedy(pat + 1, p, text, index_arr, &go_ahead);
						return match_letter(p + 1, text + go_ahead, index_arr);
					} else if (*text != '\0' && ptr[1] == '-' && *text >= ptr[0] && *text <= ptr[2]) {
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

	// Check if the next character in the pattern is an escape character
	if (*pat == '\\') {
		pat++;
		escape_flag = 1;
	}

	if (pat[1] == '*') {
		if (pat[2] == '?') greedy = 0;
		star_or_plus = 1;
	} else if (pat[1] == '+') {
		if (pat[2] == '?') greedy = 0;

		// Checking if at least one character matches
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

			// If there are no matches of the pattern after ? and the text after the current character, that means it is 0
			// for the current text, but 1 after incrementing text
			// If there is a match, the text moves ahead
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

	// If there is a star or a plus in the pattern
	if (star_or_plus) {
		int go_ahead = 0;	// A counter signifying how much the text should move forward after the greedy or non-greedy match

		if (!greedy) {
			index_arr = match_star_plus(pat, text, index_arr, escape_flag, &go_ahead);
			return match_letter(pat + 3, text + go_ahead, index_arr);
		}

		if (greedy) {
			index_arr = match_star_plus_greedy(pat, text, index_arr, escape_flag, &go_ahead);
			return match_letter(pat + 2, text + go_ahead, index_arr);
		}
	}

	// If the pattern's current character matches the text's current character
	if (!escape_flag && *text != '\0' && (*pat == '.' || *pat == *text)) {
		index_arr[2] += 1;
		return match_letter(pat + 1, text + 1, index_arr);
	}

	return index_arr;
}

// Function to match character class followed by a star or plus (non-greedy)
#if 1
int* match_character_class_star_plus(char *pat, char *p, char *text, int *index_arr, int *go_ahead)
{
	if (*text != '\0') {
		if (p[2] == '\0') return index_arr; 	// Since it is a non-greedy match, it can match nothing too

		char *ptr = pat;

		while (*ptr != ']') {
			int temp_arr[3] = {0, 0, 0};
			int *temp_match = match_letter(p + 2, text, temp_arr);

			if (temp_match[0] == 1) break;  	// If the pattern and text match, you should break

			if (*text != '\0' && (*text == *ptr || *ptr == '.')) {
				index_arr[2] += 1;
				(*go_ahead)++;
				ptr = pat;	// Resetting the pointer back to the first character to start matching in the character class
				text++;
			} else if (*text != '\0' && ptr[1] == '-' && *text >= ptr[0] && *text <= ptr[2] && ptr[2] != '\0') {
				index_arr[2] += 1;
				(*go_ahead)++;
				ptr = pat;
				text++;
			} else if (*text != '\0' && ptr[1] == '-' && (*text < ptr[0] || *text > ptr[2]) && ptr[2] != '\0') {
				ptr += 3; 	// Move the pointer ahead of the range inside the character class
			} else {
				ptr += 1; 	// Move the pointer to the next character for matching
			}
		}
		return index_arr;
	} else {
		return index_arr;
	}
}
#endif

// Function to match character class followed by a star or plus (greedy)
#if 1
int* match_character_class_star_plus_greedy(char *pat, char *p, char *text, int *index_arr, int *go_ahead)
{
	if (*text != '\0') {
		char *t = text;
		char *ptr = pat;

		// This while block is to move a pointer t over text till the last match, hence proving to be greedy
		while (*ptr != ']') {
			if (*t != '\0' && (*t == *ptr || *ptr == '.')) {
				(*go_ahead)++;
				ptr = pat;
				t++;
			} else if (*t != '\0' && ptr[1] == '-' && *t >= ptr[0] && *t <= ptr[2] && ptr[2] != '\0') {
				(*go_ahead)++;
				ptr = pat;
				t++;
			} else if (*t != '\0' && ptr[1] == '-' && (*t < ptr[0] || *t > ptr[2]) && ptr[2] != '\0') {
				ptr += 3;
			} else {
				ptr += 1;
			}
		}

		// This do-while block is to start looking for a last pattern match folllowing the character class
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
		// If condition to check whether the character has been escaped or not
		if (!escape_flag) {
			char *t; 	// Temporary pointer going over text

			for (t = text; *t != '\0' && (*t == *pat || *pat == '.'); ++t, ++(*go_ahead));	// Going all the way eager

			// Checks for the last occurence of the pattern following * in the text
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 2, t, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (t > text) {
					(*go_ahead)--;
				}
			} while (t-- > text);

			index_arr[2] += *go_ahead;
			return index_arr;
		} else {
			char *t;

			for (t = text; *t != '\0' && isValidEscapeCharacter(*pat, *t); ++t, ++(*go_ahead));
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 2, t, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (t > text) {
					(*go_ahead)--;
				}
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
		if (pat[3] == '\0') return index_arr; 	// Since it is a non-greedy match, it can match nothing too

		// If condition to check whether the character has been escaped or not
		if (!escape_flag) {
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 3, text, temp_arr);
				if (temp_match[0] == 1) {
					break; 	// Breaking at a match since it is non-greedy
				} else if (*text != '\0' && (*text == *pat || *pat == '.')) {
					(*go_ahead)++;
				}
			} while (*text != '\0' && (*text++ == *pat || *pat == '.'));

			index_arr[2] += *go_ahead;
			return index_arr;
		} else {
			do {
				int temp_arr[3] = {0, 0, 0};
				int *temp_match = match_letter(pat + 3, text, temp_arr);
				if (temp_match[0] == 1) {
					break;
				} else if (*text != '\0' && isValidEscapeCharacter(*pat, *text)) {
					(*go_ahead)++;
				}
			} while (*text != '\0' && isValidEscapeCharacter(*pat, *text++));

			index_arr[2] += *go_ahead;
			return index_arr;
		}
	} else {
		return index_arr;
	}
}
#endif
