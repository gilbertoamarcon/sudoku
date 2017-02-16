#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define PROBLEM_FILE	"repository.txt"
#define BUFFER_SIZE		255
#define M				9

#define MASK_ONE		0x0001
#define MASK_NINE		0x0100
#define MASK_NONE		0x0000
#define MASK_ALL		0x01FF
#define NUM_START		48
#define ALPHA_START		97

// Get domain size
int domain_size(int domain);

// Get only domain value
char domain_value(int domain);

// Check if value is in domain
int check_in_domain(int domain, char value);

// Remove value from domain
void remove_from_domain(int domain[M*M], int variable, char value);

// Check if all variables have values assigned
int check_complete(char values[M*M]);

// Check if all constraints are satisfied
int check_satisfaction(int domain[M*M]);

// Return the most constrained variable.
int most_constrained_variable(char values[M*M], int domain[M*M]);

// Apply all rules
int apply_rules(char values[M*M], int domain[M*M]);

// Backtracking search
int backtrack(char values[M*M], int domain[M*M]);

// Remove from the domain of each cell, a value x if it is assigned to some other cell in the same column, row, or box.
void rule_1(char values[M*M], int domain[M*M]);

// Assign to any cell a value x if it is the only value left in its domain.
void rule_2(char values[M*M], int domain[M*M]);

// Assign to any cell a value x if x is not in the domain of any other cell in that row (column or box).
void rule_3(char values[M*M], int domain[M*M]);


// Print variable status
void print_var(char values[M*M], int domain[M*M], int var);

// Print domain
void print_domain(int domain);

// Print values and domain
void print_state(char values[M*M],int domain[M*M]);

// Loading problem file
int load_problems(char *filename, char values[M*M], int domain[M*M], int problem_number);

int main(int argc, char **argv){

	// Line arguments
	int problem_number = 1;
	int verbose = 0;
	if(argc == 2){
		if(argv[1][0] == '-' && argv[1][1] == 'v') verbose = 1;
		else problem_number = atoi(argv[1]);
	}
	if(argc == 3){
		if(argv[1][0] == '-' && argv[1][1] == 'v'){
			verbose = 1;
			problem_number = atoi(argv[2]);
		}
		else{
			problem_number = atoi(argv[1]);
			if(argv[2][0] == '-' && argv[2][1] == 'v')
				verbose = 1;
		}
	}

	// Memory allocation
	char values[M*M];
	int domain[M*M];

	// Loading problem
	load_problems(PROBLEM_FILE,values,domain,problem_number);
	apply_rules(values,domain);

	// Print original values and domain
	if(verbose) print_state(values,domain);

	// CSP search
	if(backtrack(values,domain))
		printf("SUCCESS!\n");
	else
		printf("FAILURE!\n");

	// Print final values and domain
	if(verbose) print_state(values,domain);

	return 0;
}

// Get domain size
int domain_size(int domain){
	int size = 0;
	for(int i = 0; i < M; i++){
		size += (domain&MASK_NINE) > 0;
		domain <<= 1;
	}
	return size;
}

// Get first value in domain 
char domain_value(int domain){
	for(int i = 0; i < M; i++){
		if ((domain&MASK_ONE) > 0)
			return i+1+NUM_START;
		domain >>= 1;
	}
	return '-';
}

// Check if value is in domain
int check_in_domain(int domain, char value){
	if(value == '-') return 0;
	if(((domain >> (value-1-NUM_START))&MASK_ONE) > 0) return 1;
	return 0;
}

// Remove value from variable domain
void remove_from_domain(int domain[M*M], int variable, char value){
	if(value == '-') return;
	domain[variable] &= ~(MASK_ONE << (value-1-NUM_START));
}

// Check if all variables have values assigned
int check_complete(char values[M*M]){
	for(int i = 0; i < M*M; i++)
	if(values[i] == '-')
		return 0;
	return 1;
}

// Check if all constraints are satisfied
int check_satisfaction(int domain[M*M]){
	for(int i = 0; i < M*M; i++)
	if(domain[i] == MASK_NONE)
		return 0;
	return 1;
}

// Return the most constrained variable.
int most_constrained_variable(char values[M*M], int domain[M*M]){
	int min = 0;
	int min_dom = 1e3;
	for(int i = 1; i < M*M; i++)
	if(values[i] == '-' && domain_size(domain[i]) <= min_dom){
		min_dom = domain_size(domain[i]);
		min = i;
	}
	return min;
}

// Apply all rules
int apply_rules(char values[M*M], int domain[M*M]){

	rule_1(values,domain);
	if(!check_satisfaction(domain)) return 0;

	rule_2(values,domain);
	if(!check_satisfaction(domain)) return 0;

	// rule_3(values,domain);
	// if(!check_satisfaction(domain)) return 0;

	return 1;
}

// Backtracking search
int backtrack(char values[M*M], int domain[M*M]){

	// Termination condition
	if(check_complete(values)) return 1;

	// Backing up for backtracking 
	char values_back[M*M];
	int domain_back[M*M];
	memcpy(domain_back, domain, M*M*sizeof(int));
	memcpy(values_back, values, M*M*sizeof(char));

	// Selecting the most constrained variable
	int var = most_constrained_variable(values,domain);

	for(int i = 1; i <= M; i++)
	if(check_in_domain(domain[var],i+NUM_START)){
		values[var] = i+NUM_START;
		if(apply_rules(values,domain))
			if(backtrack(values,domain)) return 1;

		// Restoring backup 
		memcpy(domain, domain_back, M*M*sizeof(int));
		memcpy(values, values_back, M*M*sizeof(char));
	}

	// No variable value in the domain is valid
	return 0;
}

// Remove from the domain of each cell, a value x if it is assigned to some other cell in the same column, row, or box.
void rule_1(char values[M*M], int domain[M*M]){

	// Going through every set variable and clearing its value from other constrained variables
	for(int i = 0; i < M; i++)
	for(int j = 0; j < M; j++){
		char value = values[i*M+j];
		if(value != '-')
			for(int k = 0; k < M; k++){

				// Column
				if(k != i) remove_from_domain(domain,k*M+j,value);

				// Row
				if(k != j) remove_from_domain(domain,i*M+k,value);

				// Box
				int ki = i - i%3 + k/3;
				int kj = j - j%3 + k%3;
				if(ki != i || kj != j) remove_from_domain(domain,ki*M+kj,value);

			}
	}

}

// Assign to any cell a value x if it is the only value left in its domain.
void rule_2(char values[M*M], int domain[M*M]){
	for(int i = 0; i < M*M; i++)
	if(domain_size(domain[i]) == 1)
		values[i] = domain_value(domain[i]);
}

// Assign to any cell a value x if x is not in the domain of any other cell in that row (column or box).
void rule_3(char values[M*M], int domain[M*M]){

	// Going through every set variable and clearing its value from other constrained variables
	for(int i = 0; i < M; i++)
	for(int j = 0; j < M; j++)
	if(values[i*M+j] == '-'){

		int filter = MASK_ALL;

		for(int k = 0; k < M; k++){

			// Column
			if(values[k*M+j] != '-') filter &= ~domain[k*M+j];

			// Row
			if(values[i*M+k] != '-') filter &= ~domain[i*M+k];

			// Box
			int ki = i - i%3 + k/3;
			int kj = j - j%3 + k%3;
			if(values[ki*M+kj] != '-') filter &= ~domain[ki*M+kj];

		}
		if(domain_size(filter) > 0)
			values[i*M+j] = domain_value(filter);
	}

}











// =====================================================
// Loading and printing
// =====================================================

// Print variable status
void print_var(char values[M*M], int domain[M*M], int var){
	int j = var%M;
	int i = (var-j)/M;
	char c = j+ALPHA_START;
	printf("Variable %d%c, value %c, index %d, domain ", i+1, c, values[var],var);
	print_domain(domain[var]);
	printf("\n");
}

// Print domain
void print_domain(int domain){;
	for(int i = 0; i < M; i++){
		printf("%d",(domain&MASK_NINE)>0);
		domain <<= 1;
	}
	printf(" ");
}

// Print values and domain
void print_state(char values[M*M],int domain[M*M]){
	printf("   a b c   d e f   g h i");
	printf("          a         b         c           d         e         f           g         h         i");
	printf("\n");
	for(int i = 0; i < M; i++){
		if(i == 3 || i == 6) printf(" ------------------------ \n");
		printf("%d  ",i+1);
		for(int j = 0; j < M; j++){
			if(j == 3 || j == 6) printf("| ");
			printf("%c ",values[i*M+j]);
		}
		printf("  %d  ",i+1);
		for(int j = 0; j < M; j++){
			if(j == 3 || j == 6) printf("| ");
			print_domain(domain[i*M+j]);
		}
		printf("\n");
	}
}

// Loading problem file
int load_problems(char *filename, char values[M*M], int domain[M*M], int problem_number){

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

	// Parsing problem values
	for(int i = 0; i < M; i++){
		if(fgets(fileBuffer,BUFFER_SIZE,file) == NULL) return 1;
		char *r = fileBuffer;
		for(int j = 0; j < M; j++){
			values[i*M+j] = *(r++);
			domain[i*M+j] = (MASK_ONE << (values[i*M+j]-1-NUM_START));
			if(values[i*M+j] == '0'){
				values[i*M+j] = '-';
				domain[i*M+j] = MASK_ALL;
			}
			if(*r == ' ' || *r == '	') r++;
		}
	}

	fclose(file);
	return 0;
}