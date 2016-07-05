/********************************************************************
Program Filename: main.c
*
*
Author: Alex Hildreth
*
*
Date:11/28/14
** Description: Contains the main function for testing hashMap.c. 
** Takes a text file as input. Parses out each word and inserts it
** into the hash table. Finally, counts the number of times each
** word was used and prints them out.
** Input: a text file
** Output: the words and the number of times they occured, as well
** as data on the table and how the program ran.
*
********************************************************************/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashMap.h"

/*
 the getWord function takes a FILE pointer and returns you a string which was
 the next word in the in the file. words are defined (by this function) to be
 characters or numbers seperated by periods, spaces, or newlines.
 
 when there are no more words in the input file this function will return NULL.
 
 this function will malloc some memory for the char* it returns. it is your job
 to free this memory when you no longer need it.
 */
char* getWord(FILE *file);

int main (int argc, const char * argv[]) {
	const char* filename;
	struct hashMap *hashTable;	
	int tableSize = 10;
	clock_t timer;
	FILE *fileptr;	
    /*
     this part is using command line arguments, you can use them if you wish
     but it is not required. DO NOT remove this code though, we will use it for
     testing your program.
     
     if you wish not to use command line arguments manually type in your
     filename and path in the else case.
     */
    if(argc == 2)
        filename = argv[1];
    else
        filename = "input1.txt"; /*specify your input text file here*/
    
    printf("opening file: %s\n", filename);
    
	timer = clock();
	
	hashTable = createMap(tableSize);	
	/*... concordance code goes here ...*/
	
	//open file
	fileptr = fopen(filename, "r");  
	assert(fileptr != NULL);
	
	char *currentWord;
	
	while((currentWord = getWord(fileptr)) != NULL)
	{
		insertMap(hashTable, currentWord, 1);
	}
	//close file
	fclose(fileptr);
	
	//print list
	int i;
	struct hashLink *current;
	for(i = 0; i < hashTable->tableSize; i++)
	{
		if(hashTable->table[i] != NULL)
		{
			current = hashTable->table[i];
			while(current!= NULL) 
			{
				//insert into the new table
				printf("%s: %d\n", current->key, current->value);
				
				current = current->next;
			}
		}
		//printf("\n");
		//printf("END OF BUCKET\n");
		//printf("\n");
	}
	/*... concordance code ends here ...*/

	printMap(hashTable);
	timer = clock() - timer;
	printf("\nconcordance ran in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
	printf("Table emptyBuckets = %d\n", emptyBuckets(hashTable));
    printf("Table count = %d\n", size(hashTable));
	printf("Table capacity = %d\n", capacity(hashTable));
	printf("Table load = %f\n", (float)tableLoad(hashTable));
	
	printf("Deleting keys\n");
	
	removeKey(hashTable, "and");
	removeKey(hashTable, "me");
	removeKey(hashTable, "the");
	printMap(hashTable);
		
	deleteMap(hashTable);
	printf("\nDeleted the table\n");  
	
	return 0;
}

void printValue(ValueType v) {
	printf("Value:%d",(int *)v);
}

char* getWord(FILE *file)
{
	int length = 0;
	int maxLength = 16;
	char character;
    
	char* word = malloc(sizeof(char) * maxLength);
	assert(word != NULL);
    
	while( (character = fgetc(file)) != EOF)
	{
		if((length+1) > maxLength)
		{
			maxLength *= 2;
			word = (char*)realloc(word, maxLength);
		}
		if((character >= '0' && character <= '9') || /*is a number*/
		   (character >= 'A' && character <= 'Z') || /*or an uppercase letter*/
		   (character >= 'a' && character <= 'z') || /*or a lowercase letter*/
		   character == 39) /*or is an apostrophy*/
		{
			word[length] = character;
			length++;
		}
		else if(length > 0)
			break;
	}
    
	if(length == 0)
	{
		free(word);
		return NULL;
	}
	word[length] = '\0';
	return word;
}
