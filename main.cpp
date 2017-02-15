#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <ctime>

#define PROBLEM_FILE	"repository.txt"
#define PROBLEM_NUM		9
#define BUFFER_SIZE		255
#define M				9

// Loading problem file
int load_problems(char *filename, char board[M][M], int problem_number){

	char fileBuffer[BUFFER_SIZE] = "";
	FILE *file = NULL;
	file = fopen(filename,"r");
	if(file == NULL || fgets(fileBuffer,BUFFER_SIZE,file) == NULL){
		printf("Error: Origin file '%s' not found.",filename);
		return 1;
	}

	// Scrolling down to problem problem_number
	while(atoi(fileBuffer) != problem_number)
		for(int i = 0; i <= M+1; i++)
			if(fgets(fileBuffer,BUFFER_SIZE,file) == NULL) return 1;

	printf("Problem %s",fileBuffer);

	// Parsing problem board
	for(int i = 0; i < M; i++){
		if(fgets(fileBuffer,BUFFER_SIZE,file) == NULL) return 1;
		char *r = fileBuffer;
		for(int j = 0; j < M; j++){
			board[i][j] = *(r++);
			if(*r == ' ' || *r == '	') r++;
		}
	}

	fclose(file);
	return 0;
}

// Board display
void display(char board[M][M]){
	printf("   a b c   d e f   g h i\n");
	for(int i = 0; i < M; i++){
		if(i == 3 || i == 6) printf(" ------------------------\n");
		printf("%d  ",i+1);
		for(int j = 0; j < M; j++){
			if(j == 3 || j == 6) printf("| ");
			printf("%c ",board[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv){

	int problem_number = 1;
	if(argc == 2) problem_number = atoi(argv[1]);

	char board[M][M];

	load_problems(PROBLEM_FILE,board,problem_number);

	display(board);

	return 0;
}