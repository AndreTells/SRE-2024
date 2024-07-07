#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHECK_CALL(sts, msg) if (-1 == (sts) && errno != 0) 		\
	{	perror(msg); exit(EXIT_FAILURE); }			\
	else   printf("%s : %d\n", msg, sts)

#define CHECK_PTR(ptr, msg) if(ptr == NULL) 				\
	{ perror(msg); exit(EXIT_FAILURE);}				\

#define LEN_ALPHABET 26

#define TRUE 1
#define FALSE 0

#define WORDHASHSIZE 5000
int wordFoundHash[WORDHASHSIZE]={0}; // store if a word has already been checked
//function taken from: http://www.cse.yorku.ca/~oz/hash.html
unsigned long GetDJB2Hash(unsigned char* str);

// Function to check if a given sequence of words exists
// Returns TRUE if all the words in words are in dictionary and FALSE otherwise
int CheckShiftedWordsExists(char* text,int shift, char* dictionary);

// expects word to be in lower case and for shift to be a positve number
char* ShiftWord(char* word, int shift);

int main(void){
	const char* text = "uhinbyl vlcwe ch nby quff";	
	const char* dictionary = "shift test caesar dcode";
	int min_i = -1;
	int min_res = -1;
	for(int i=0; i<LEN_ALPHABET;i++){
		printf("testing: %d \n", i);
		int res = CheckShiftedWordsExists((char*)text,i,(char*)dictionary);
		min_i = (min_res<res) ? i: min_i; 
		if(res != 0) continue;
		break;
	}
	printf("sequence is shifted by %d \n",min_i);
	return EXIT_SUCCESS;
}

unsigned long GetDJB2Hash(unsigned char* str){
	unsigned long hash = 5381;
       	int c;
	while((c=*str++)){
		hash= ((hash<<5)+hash)+c;/*hash*33 +c*/
	}
	return hash;
}


int CheckShiftedWordsExists(char* text, int shift, char* dictionary){
	//Checking for error in input
	CHECK_PTR(text, "text sent to SequenceIsDecoded function was NULL");
	CHECK_PTR(dictionary, "dictionary sent to SequenceIsDecoded function was NULL");		
	
	char* refptr;
	char* textCopy = strdup(text);
	char* word = strtok_r(textCopy," ", &refptr);
	int numWrongWords = 0;
	while(word!=NULL){
		// duplicates the word and shifts it
		char* shiftedWord = ShiftWord(strdup(word), shift);
		printf("%s %s \n", word,shiftedWord);
		// check if word is in the dictionary
		//long wordHash = GetDJB2Hash((unsigned char*)shiftedWord) % WORDHASHSIZE;
		//if(wordFoundHash[wordHash]==FALSE)
		if(strstr(dictionary, shiftedWord)==NULL)numWrongWords++;	
		//wordFoundHash[wordHash] = TRUE;
		
		//frees space allocated for the word duplicate
		free(shiftedWord);
		word = strtok_r(NULL," ",&refptr);
	}	
	free(textCopy);
	return numWrongWords;	
}

char* ShiftWord(char* word, int shift){
	// check for error in input
	CHECK_PTR(word, "word sent to ShiftWord function was NULL");

	// set constants necessary for algorithm
	const char* letters ="abcdefghijklmnopqrstuvxywz";

	char* wordLetter = word;
	while(*wordLetter !='\0'){
		int currentIndex = (*wordLetter-'a');
		int shiftedIndex = (currentIndex+shift) % LEN_ALPHABET;
		*wordLetter = letters[shiftedIndex];
		wordLetter++;
	}
	return word;
}
