#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re.h"

#define MAX 5000

int main()
{
	char text[MAX];
	char pattern[MAX];

	printf("Input text:\n");
	fgets(text, MAX, stdin);
	text[strlen(text) - 1] = '\0';

	printf("Input pattern:\n");
	fgets(pattern, MAX, stdin);
	pattern[strlen(pattern) - 1] = '\0';

	int *match_arr = match(pattern, text);

	if (match_arr && match_arr[0] != 0) {
		for (int i=0; i<3; ++i) {
			printf("%d ", match_arr[i]);
		}
		printf("\n");
	} else if (match_arr && match_arr[0] == 0) {
		printf("%d\n", 0);
	} else {
		printf("NULL pointer received.\n");
	}

	return 0;
}
