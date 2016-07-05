/********************************************************************
Program Filename: hashMap.c
*
*
Author: Alex Hildreth
*
*
Date:11/28/14
** Description: Implimentation of hashMap.h. Creates and
** manages a hash map data structure using buckets. Allows addition and
** removal of links consisting of a key and a value, retreival of data, 
** and search functionality. Hashes each key into the table. Resizes
** and rehashes all links when necessary.
** Input: n/a
** Output: n/a
*
********************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashMap.h"



/*the first hashing function you can use*/
int stringHash1(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += str[i];
	return r;
}

/*the second hashing function you can use*/
int stringHash2(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += (i+1) * str[i]; /*the difference between stringHash1 and stringHash2 is on this line*/
	return r;
}

/* initialize the supplied hashMap struct*/
void _initMap (struct hashMap * ht, int tableSize)
{
	int index;
	if(ht == NULL)
		return;
	ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
	ht->tableSize = tableSize;
	ht->count = 0;
	for(index = 0; index < tableSize; index++)
		ht->table[index] = NULL;
}

/* allocate memory and initialize a hash map*/
hashMap *createMap(int tableSize) {
	assert(tableSize > 0);
	hashMap *ht;
	ht = malloc(sizeof(hashMap));
	assert(ht != 0);
	_initMap(ht, tableSize);
	return ht;
}

/*
 Free all memory used by the buckets.
 Note: Before freeing up a hashLink, free the memory occupied by key and value
 */
void _freeMap (struct hashMap * ht)
{  
	int i;
	for(i = 0; i < ht->tableSize; i++)
	{
		if(ht->table[i] != NULL)
		{
			struct hashLink * current = ht->table[i];
			struct hashLink * next = current->next;
			
			while(current != NULL)
			{
				next = current->next;
				free(current->key);
				current->value = 0;
				free(current);
				
				current = next;
			}
		}
	}		
}

/* Deallocate buckets and the hash map.*/
void deleteMap(hashMap *ht) {
	assert(ht!= 0);
	/* Free all memory used by the buckets */
	_freeMap(ht);
	/* free the hashMap struct */
	free(ht);
}

/* 
Resizes the hash table to be the size newTableSize 
*/
void _setTableSize(struct hashMap * ht, int newTableSize)
{
	//make new, larger table
	struct hashMap *newMap;
	newMap = createMap(newTableSize);
	//assert(newMap != NULL);
	
	struct hashLink *current;
	struct hashLink *next;
	
	//rehash and distribute hashLinks
	int i;
	for(i = 0; i < ht->tableSize; i++)
	{
		if(ht->table[i] != NULL)
		{
			current = ht->table[i];
			while(current!= NULL) 
			{
				//insert into the new table
				insertMap(newMap, current->key, current->value);
				
				current = current->next;
			}
		}
	}
	
	//delete old map and set ht to new one
	free(ht->table);
	ht->table = newMap->table;
	ht->tableSize = newMap->tableSize;	
}

/*
 insert the following values into a hashLink, you must create this hashLink but
 only after you confirm that this key does not already exist in the table. For example, you
 cannot have two hashLinks for the word "taco".
 
 if a hashLink already exists in the table for the key provided you should
 replace that hashLink--this requires freeing up the old memory pointed by hashLink->value
 and then pointing hashLink->value to value v.
 
 also, you must monitor the load factor and resize when the load factor is greater than
 or equal LOAD_FACTOR_THRESHOLD (defined in hashMap.h).
 */
void insertMap (struct hashMap * ht, KeyType k, ValueType v)
{  
	//get hash value
	int hashIdx;
	if(HASHING_FUNCTION == 1)
	{
		hashIdx	= stringHash1(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	else
	{
		hashIdx	= stringHash2(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	
	//check if already in table
	int alreadyContains = containsKey(ht, k);
	if(alreadyContains == 1)
	{
		//increase value if already in table
		struct hashLink * current = ht->table[hashIdx];
		if(strcmp(current->key, k) == 0)
			{
				current->value += 1;
				return;
			}
		
		while(strcmp(current->key, k) != 0)
		{
			
			current = current->next;
			if(strcmp(current->key, k) == 0)
			{
				current->value += 1;
				return;
			}
		}
	}
	
	//check if resize necessary first
	if(tableLoad(ht) > LOAD_FACTOR_THRESHOLD)
	{
		_setTableSize(ht, (ht->tableSize * 2));
		
		//recalculate hash idx
		if(HASHING_FUNCTION == 1)
		{
			hashIdx	= stringHash1(k) % ht->tableSize;
			if(hashIdx < 0) hashIdx += ht->tableSize;
		}
		else
		{
			hashIdx	= stringHash2(k) % ht->tableSize;
			if(hashIdx < 0) hashIdx += ht->tableSize;
		}
	}
	
	//check if already a bucket at location
	if(ht->table[hashIdx] != NULL)
	{
		struct hashLink * newLink = (struct hashLink*)malloc(sizeof(struct hashLink));
		assert(newLink);
		
		newLink->value = v;
		newLink->key = k;
		newLink->next = ht->table[hashIdx];
		ht->table[hashIdx] = newLink;
		ht->count++;
	}
	//otherwise, make the link a new bucket at location
	else
	{
		struct hashLink * newLink = (struct hashLink*)malloc(sizeof(struct hashLink));
		assert(newLink);
		
		newLink->value = v;
		newLink->key = k;
		newLink->next = NULL; //will be last link in chain
		ht->table[hashIdx] = newLink;
		ht->count++;
	}
}

/*
 this returns the value (which is void*) stored in a hashLink specified by the key k.
 
 if the user supplies the key "taco" you should find taco in the hashTable, then
 return the value member of the hashLink that represents taco.
 
 if the supplied key is not in the hashtable return NULL.
 */
ValueType* atMap (struct hashMap * ht, KeyType k)
{ 
	//get hash value
	int hashIdx;
	if(HASHING_FUNCTION == 1)
	{
		hashIdx	= stringHash1(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	else
	{
		hashIdx	= stringHash2(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	
	//check if in table
	int contains = containsKey(ht, k);
	if(contains == 0)
	{
		printf("No such key found.\n");
		return NULL;
	}
	
	struct hashLink * current = ht->table[hashIdx];
	struct hashLink * next = current->next;
	
	while(current->key != k)
	{
		current = next;
		next = current->next;
	}
	ValueType retVal = (ValueType)current->value;
	return retVal;
}

/*
 a simple yes/no if the key is in the hashtable. 
 0 is no, all other values are yes.
 */
int containsKey (struct hashMap * ht, KeyType k)
{  
	//get hash value
	int hashIdx;
	if(HASHING_FUNCTION == 1)
	{
		hashIdx	= stringHash1(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	else
	{
		hashIdx	= stringHash2(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	
	//if no bucket at location, return 0
	if(ht->table[hashIdx] == NULL)
	{
		return 0;
	}
	
	//otherwise, look through location
	struct hashLink * current = ht->table[hashIdx];
	
	while(current != NULL) 
	{
		if(strcmp(current->key, k) == 0)
		{
			return 1;
		}
		
		current = current->next;
	}
	
	//if it gets here, key is not in table.
	return 0;
}

/*
 find the hashlink for the supplied key and remove it, also freeing the memory
 for that hashlink. it is not an error to be unable to find the hashlink, if it
 cannot be found do nothing (or print a message) but do not use an assert which
 will end your program.
 */
void removeKey (struct hashMap * ht, KeyType k)
{  
	//check if in table
	if(containsKey(ht, k) == 0)
	{
		printf("No such key found.\n");
		return;
	}
	
	//get hash value
	int hashIdx;
	if(HASHING_FUNCTION == 1)
	{
		hashIdx	= stringHash1(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	else
	{
		hashIdx	= stringHash2(k) % ht->tableSize;
		if(hashIdx < 0) hashIdx += ht->tableSize;
	}
	
	//if the first member is the key in question, deal with separately
	struct hashLink * current = ht->table[hashIdx];
	if(strcmp(current->key, k) == 0)
	{
		ht->table[hashIdx] = current->next;
		free(current->key);
		current->value = 0;
		free(current);
		return;
	}
	
	
	struct hashLink * prev = current;
	current = current->next;
	
	while(strcmp(current->key, k) != 0)
	{
		prev = current;
		current = current->next;
	}
	
	//close gap and free current
	prev->next = current->next;
	free(current->key);
	current->value = 0;
	free(current);	
}

/*
 returns the number of hashLinks in the table
 */
int size (struct hashMap *ht)
{  
	return ht->count;
	
}

/*
 returns the number of buckets in the table
 */
int capacity(struct hashMap *ht)
{  
	return ht->tableSize;
}

/*
 returns the number of empty buckets in the table, these are buckets which have
 no hashlinks hanging off of them.
 */
int emptyBuckets(struct hashMap *ht)
{  
	int emptyCount = 0;
	int i;
	for(i = 0; i < ht->tableSize; i++)
	{
		if(ht->table[i] == NULL)
		{
			emptyCount++;
		}
	}
	return emptyCount;
}

/*
 returns the ratio of: (number of hashlinks) / (number of buckets)
 
 this value can range anywhere from zero (an empty table) to more then 1, which
 would mean that there are more hashlinks then buckets (but remember hashlinks
 are like linked list nodes so they can hang from each other)
 */
float tableLoad(struct hashMap *ht)
{  
	float theLoad = (float)(ht->count) / (float)(ht->tableSize);
	return theLoad;
}


void printMap (struct hashMap * ht)
{
	int i;
	struct hashLink *temp;	
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		if(temp != 0) {		
			printf("\nBucket Index %d -> ", i);		
		}
		while(temp != 0){			
			printf("Key:%s|", temp->key);
			printValue(temp->value);
			printf(" -> ");
			temp=temp->next;			
		}		
	}
}


