/**
 * Crypto Tweet hash.c
 *
 * CPSC 441, Assignment 2
 * Glenn Skelton
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "definitions.h"



/**
 * Purpose: To search a hash map given the key value. If the value exists, return
 * the item else return -1 to signify not found.
 */
int search(char *key, char **hashTable) {
	int val = -1;
	// search the table
	for (int i = 0; i < MAX_VAL; i++) {
		if (hashTable[i] != NULL) {
			if (strcmp(hashTable[i], key) == 0) {
				val = i;
				break;
			}
		}
	}
	// if the item is there return it else return -1
	return val;
}

/**
 * Purpose: To search a hash map given a value. If the item exists, return the key value pair
 * else return NULL (which is the default stored value) if not found.
 */
char *searchValue(int val, char **hashTable) {
	return hashTable[val]; // if nothing is there it will just return null
}


/**
 * Purpose: To insert a new key value pair into the hash map. If the item already exists, do
 * nothing. Else, if the item does not exist, apply the hashing function passed in and store
 * the result. If the hash calculated hashes to an index that is already being used then apply
 * quadratic probing to try and find the next available slot.
 */
int insert(char *key, int (*f)(char*), char **hashTable) {
	// hash the item
	unsigned short val = 0;

	// check to make sure there is still room to add items
	int bool = 0;
	for (int i = 0; i < MAX_VAL; i++) {
		if (hashTable[i] == NULL) {
			bool = 1;
			break;
		}
	}
	// if full, exit
	if (bool == 0) {
		return -1; // signifies full error
	}
	// there is an empty spot so add it
	int value = search(key, hashTable);
	if (value >= 0) { // check to see if item is in table
		return value; // return the value from the key/value pair (index)
	}
	else { // new item so hash it
		val = f(key); // hash the key using the function passed in (function pointer)
		val = val % MAX_VAL; // reduce key size to that of the table

		// check to make sure nothing is there
		while (hashTable[val] != NULL) {
			//val = (val+1) % MAX_VAL; // linear probing
			val = (val*val) % MAX_VAL; // quadratic probing
		}
		// add item to the table at the index specified
		if ((hashTable[val] = malloc(sizeof(char)*strlen(key)+1)) == NULL) {
			fprintf(stderr, "Error allocating memory for item\n");
			exit(EXIT_FAILURE);
		}
		strcpy(hashTable[val], key); // copy the item into the array to return
		return val;
	}
}


/**
 * Purpose: To print out the hash table key value pairs
 */

void printHashTable(char **hashTable) {
	for (int i = 0; i < MAX_VAL; i++) {
		if (hashTable[i] != NULL) { // retrieve only hashes that have keys/value pairs
			printf("0x%04x\t%s\n", i, hashTable[i]);
		}
	}
}


/************************************ HASHES *************************************/

/**
 * Purpose: To sequentially hash a key based on an incrementing global value.
 */
int seqHash(char *key) {
	if (seqHashVal+1 < MAX_VAL) {
		seqHashVal++; // increment global hash variable
		return seqHashVal;
	}
	else return -1; // signifies error if table has reached maximum size
}


/**
 * Purpose: To hash a key using the sum of its characters. Has an inherent issue that
 * on the off chance two words hash the same value, depending on which was hashed first
 * they ordering could be changed.
 */
int wordSumHash(char *key) {
	// search through and get the letters of the word
	int sum = 0, i = 0;
	while (key[i] != '\0') {
		sum += key[i++]; // get the character's ascii value and add it to sum
	}
	return sum;
}


/**
 * Purpose: To take a key and produce a hash value. The value is calculated by taking the
 * ascii value of each character and multiply by its reverse position in the string. Ie.
 * the highest order byte will be multiplied by the highest index and decrement so forth.
 * N is the string length and i is the index in the string
 * For example "Mary" would become (0x4D * 4-0) + (0x61 * 4-1) + (0x72 * 4-2) + (0x79 * 4-3)
 *
 * inspiration from:
 * interactivepython.org/runestone/static/pythonds/SortSearch/Hashing.html
 *
 * anagrams produce different values
 * has issue if the words are the same length and the ordering of the powers causes the sum
 * to be the same ie. he == as, she == one etc.
 */
int myHash(char *key) {
	int i = 0, total = 0;
	unsigned short tmp; // tmp is 2 bytes
	int n = strlen(key);

	while (key[i] != '\0') {
		tmp = key[i]*(n-i); // multiply the value of each character by its high order position
		total += tmp; // add the calculated value to the total
		i++;
	}
	return total;
}


