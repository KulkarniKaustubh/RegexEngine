#ifndef RE
#define RE

int* match(char *pat, char *text);
int* match_letter(char *pat, char *text, int *index_arr);
int* match_star_plus(char *pat, char *text, int *index_arr, int escape_flag, int *go_ahead);

#endif
