#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define MAX 2000

// FUNCTION PROTOTYPES:

// Returns greater value between given two:
int max(int a, int b);

// Creates a case sensitive bad match table referring to given pattern:
void createTableCS(char pattern[], int patternLength, int bmBc[]);

// Creates a non case sensitive bad match table referring to given pattern:
void createTableNonCS(char pattern[], int patternLength, int bmBc[]);

// Shifts string left strarting from given index by shiftCount.
void shiftLeft(char str[], int index, int shiftCount);

// Shifts string left strarting from given index by shiftCount.
void shiftRight(char str[], int index, int shiftCount);

// Replaces the string replace[] into given index in string text[]:
void replaceText(char text[], char replace[], int index, int patternSize);

// Finds all patterns in given string and replaces them by using bmBc table (case sensitive):
int findAndReplaceCS(char text[], char pattern[], char replace[], int bmBc[]);

// Finds all patterns in given string and replaces them by using bmBc table (non case sensitive):
int findAndReplaceNonCS(char text[], char pattern[], char replace[], int bmBc[]);

// MAIN FUNCTION:

int main(){
	// Declarations:
	char text[MAX], backup[MAX], pattern[MAX], replace[MAX], fileName[MAX], option;
	int bmBc[MAX], patternSize, replaceSize,  replaceCount;
	double runningTime;
	struct timeval start, stop;
	FILE *fp;
	
	// 1) User gives inputs:
	printf("Please enter the pattern that you wish to find:\n");
	gets(pattern);
	printf("Please enter the pattern that you wish to replace:\n");
	gets(replace);
	printf("Please enter the name of the file to process:\n");
	gets(fileName);
	printf("Case senstive? (Enter 'Y' for yes, 'N' for no):\n");
	scanf("%c",&option);
	
	// 2.a) Read text from the file and save it to memory: 
	fp = fopen(fileName,"r");
	fgets(text,MAX,fp);
	fclose(fp);
	strcpy(backup,text);	// Backup of unprocessed text
	
	// 2.b) Find and replace:
	gettimeofday(&start, NULL);		//	Start for time measurement
	patternSize = strlen(pattern);
	//	Case sensitive:
	if(option == 'y' || option == 'Y'){
		createTableCS(pattern, patternSize, bmBc);
		replaceCount = findAndReplaceCS(text,pattern,replace,bmBc);
	}
	//	Non case sensitive::
	else{
		createTableNonCS(pattern, patternSize, bmBc);
		replaceCount = findAndReplaceNonCS(text,pattern,replace,bmBc);
	}
	
	// 2.c) Overwrite text to file:
	fp = fopen(fileName,"w");
	fprintf(fp,"%s",text);
	fclose(fp);
	gettimeofday(&stop, NULL);		// End for time measurement
	
	// 3) Process report:
	runningTime = (double)1000*(double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);	// Time measurement
	printf("\nFind: %s\n\nReplace: %s\n\n", pattern, replace);
	if(option == 'y' || option == 'Y')
		printf("Option: Case sensitive\n\n");
	else
		printf("Option: Not case sensitive\n\n");
	printf("Text: %s\nNew Text: %s\nFound and Replaced: %d\n\nRunning Time: %f milliseconds", backup, text, replaceCount, runningTime);
	
	return 0;
}

//	FUNCTION DEFINITIONS:

int max(int a, int b){
	if(a > b)
		return a;
	else
		return b;
}

void createTableCS(char pattern[], int patternLength, int bmBc[]){
	int i;
	// Initialize all values in the table with patternLength:
	for(i=0; i<MAX; i++){
		bmBc[i] = patternLength;
	}
	// Calculate and assign table values of characters which exist in the pattern:
	for(i=0; i<patternLength; i++){
		bmBc[pattern[i]] = max(1, patternLength-i-1);
	}
}

void createTableNonCS(char pattern[], int patternLength, int bmBc[]){
	int i;
	int caseDifference = 'a'-'A';
	// Initialize all values in the table with patternLength:
	for(i=0; i<MAX; i++){
		bmBc[i] = patternLength;
	}
	// Calculate and assign table values of characters which exist in the pattern:
	for(i=0; i<patternLength; i++){
		bmBc[pattern[i]] = max(1, patternLength-i-1);
		if(pattern[i] >= 'A' && pattern[i] <= 'Z')
			bmBc[pattern[i]+caseDifference] = max(1, patternLength-i-1);
		else if(pattern[i] >= 'a' && pattern[i] <= 'z')
			bmBc[pattern[i]-caseDifference] = max(1, patternLength-i-1);
	}
}

void shiftLeft(char str[], int index, int shiftCount){
	int i;
	int strSize = strlen(str);
	for(i=index; i<strSize; i++){
		str[i-shiftCount] = str[i];
	}
	str[strSize-shiftCount] = '\0';
}

void shiftRight(char str[], int index, int shiftCount){
	int i;
	int strSize = strlen(str);
	for(i=strSize-1; i>=index; i--){
		str[i+shiftCount] = str[i];
	}
	str[strSize+shiftCount] = '\0';
}

void replaceText(char text[], char replace[], int index, int patternSize){
	int replaceSize = strlen(replace);
	int shiftCount = replaceSize - patternSize;
	int i = 0;
	// Shift left:
	if(replaceSize < patternSize){
		shiftLeft(text, index+patternSize, abs(shiftCount));
	}
	// Shift right:
	else if(replaceSize > patternSize){
		shiftRight(text, index+patternSize, shiftCount);
	}
	// Replace:
	for(i=0; i<replaceSize; i++){
		text[index+i] = replace[i];
	}
}

int findAndReplaceCS(char text[], char pattern[], char replace[], int bmBc[]){
	int replaceCount = 0;
	int textSize = strlen(text);
	int patternSize = strlen(pattern);
	int i=0, j, count = 0;
	while(i <= textSize-patternSize){
		j = patternSize - 1;
		// Rightmost characters matched:
		if(pattern[j] == text[i+j]){
			// Checking every single charater in pattern until a mismatch:
			while (j>=0 && pattern[j] == text[i+j]){
				j--;
			}
			// All pattern mached:
			if(j == -1){
				replaceText(text, replace, i, patternSize);
				replaceCount++;
				i = i+patternSize;
				textSize = strlen(text);
			}
			// Pattern not matched, skip by the table value and continue:
			else{
				i = i+bmBc[pattern[patternSize-1]];
			}
		}
		// Rightmost characters mismatch, skip by the table value and continue:
		else{
			i = i+bmBc[text[i+j]];
		}
	}
	return replaceCount;
}

int findAndReplaceNonCS(char text[], char pattern[], char replace[], int bmBc[]){
	int caseDifference = 'a' - 'A';
	int replaceCount = 0;
	int textSize = strlen(text);
	int patternSize = strlen(pattern);
	int i=0, j, count = 0;
	while(i <= textSize-patternSize){
		j = patternSize - 1;
		// Rightmost characters matched:
		if(pattern[j] == text[i+j] || (abs(pattern[j]-text[i+j]) == caseDifference && pattern[j] >= 'A' && pattern[j] <= 'z' && text[i+j] >='A' && text[i+j] <= 'z')){
			// Checking every single charater in pattern until a mismatch:
			while (j>=0 && (pattern[j] == text[i+j] || (abs(pattern[j]-text[i+j]) == caseDifference && pattern[j] >= 'A' && pattern[j] <= 'z' && text[i+j] >='A' && text[i+j] <= 'z'))){
				j--;
			}
			// All pattern mached:
			if(j == -1){
				replaceText(text, replace, i, patternSize);
				replaceCount++;
				textSize = strlen(text);
				i = i+patternSize;
			}
			// Pattern not matched, skip by the table value and continue:
			else{
				i = i+bmBc[pattern[patternSize-1]];
			}
		}
		// Rightmost characters mismatch, skip by the table value and continue:
		else{
			i = i+bmBc[text[i+j]];
		}
	}
	return replaceCount;
}
