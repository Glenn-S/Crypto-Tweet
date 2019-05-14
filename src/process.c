/**
 * Crypto Tweet process.c
 *
 * CPSC 441, Assignment 2
 * Glenn Skelton
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "hash.h"
#include "definitions.h"


/**
 * Purpose: To check if a string needs to be encoded or decoded and pass the message on
 * to the appropriate function
 */
char *parseString(char *message, char **hashTable) {
	char *retMsg = NULL, *p;

	p = strstr(message, "0x");
	if ((p-message) != 0) { // if the first characters aren't "0x" encode the message
		retMsg = encode(message, hashTable);
	}
	else { // if "0x" is at the beginning of the message
		retMsg = decode(message, hashTable);
	}
#if DEBUG
	printHashTable(hashTable);
#endif
	return retMsg;

}

/**
 * Purpose: To encode a tweet that is passed in a string. Each word is split and the punctuation
 * is removed for storing. Upon return, the encrypted value is rejoined with any punctuation
 * that existed in the word previously. All of the words encrypted values are stored back into
 * one string in the order that they were received with the correct punctuation
 */
char *encode(char *msg, char **hashTable) {
	// split based on space, check for punctuation that is not in the middle
	char *words[MAX_TWEET], *token, *ret;
	memset(words, 0, sizeof(MAX_TWEET)); // zero out the memory for words
	ret = NULL;

	// split into tokens based on " "
	token = strtok(msg, " ");
	int size = 0;
	while (token != NULL) {
		words[size] = token;
		token = strtok(NULL, " ");
		size++;
	}

	// set up buffer for returning values
	int value;
	char buffer[1000], tmp[7];
	memset(buffer, 0, sizeof(buffer));
	memset(tmp, 0, sizeof(tmp));

	// loop through each word and seperate punctuation and encrypt or return already encrypted value
	for (int i = 0; i < size; i++) {
		// create storage for possible punctuation
		char front[MAX_TWEET+1], back[MAX_TWEET+1];
		memset(front, 0, sizeof(front));
		memset(back, 0, sizeof(back));
		int j = 0;
		int bool = 0;

		/*********************** CHECK WORD FOR PUNCTUATION **************************/

		// check to make sure the word actually has alphanumeric characters
		while (words[i][j] != '\0') {
			if (isalnum(words[i][j]) > 0) {
				bool = 1; // there is at least one alphanumeric character in the word
				break;
			}
			j++;
		}
		// check to see if it is only made of punctuation and add to return string if so
		if (bool == 0) {
			strcat(buffer, words[i]); // add the punctuation back in if it is rogue
			strcat(buffer, " ");
			continue; // go to the next word, this is just rogue punctuation
		}

		int first = 0, last = 0;
		// loop through and check for front punctuation
		while (words[i][j] != '\0') {
			if (isalnum(words[i][j]) > 0) {
				first = j; // get the index of the first alphanumeric character
				break;
			}
			j++;
		}
		if (first > 0) {
			strncpy(front, words[i], first); // copy non numeric characters into front string
			words[i] = words[i]+first;
		}
		// loop through and check for back punctuation (ignore stuff in the middle)
		j = 0;
		while (words[i][j] != '\0') { // search for the last alpha numeric character
			if (isalnum(words[i][j]) > 0) {
				last = j; // record the current last alpha numeric character
			}
			j++;
		}
		if (last < strlen(words[i])-1) {
			strncpy(back, words[i]+last+1, strlen(words[i])-last);
			words[i][last+1] = '\0';
		}
		strcat(buffer, front); // add the front punctuation

		/************************* ENCRYPTION TIME *************************/
		// get the encryption
		int itemVal = search(words[i], hashTable); // search to see if word is in hash map
		if (itemVal < 0) { // item is not in the list so hash and store
			switch (ENCRYPT) {
			case 1:
				value = insert(words[i], seqHash, hashTable); // calculate and store the key value pair
				sprintf(tmp, "0x%04x", value); // tmp has the string version now
				break;
			case 2:
				value = insert(words[i], wordSumHash, hashTable); // calculate and store the key value pair
				sprintf(tmp, "0x%04x", value); // tmp has the string version now
				break;

			case 3:
				value = insert(words[i], myHash, hashTable); // calculate and store the key value pair
				sprintf(tmp, "0x%04x", value); // tmp has the string version now
				break;
			}

		} // if item already exists in the table, return its previously hashed value
		else if (itemVal > 0) {
			sprintf(tmp, "0x%04x", itemVal); // store formatted hex string
		}
		// store the word and its punctuation that comes after
		strcat(buffer, tmp); // store encoded word segment into larger buffer temporarily
		strcat(buffer, back); // add the end back on to the word
		strcat(buffer, " ");

	}
	// allocate memory for return pointer
	if ((ret = malloc(sizeof(buffer))) == NULL) {
		fprintf(stderr, "Error allocating memory\n");
		exit(EXIT_FAILURE);
	}
	strncpy(ret, buffer, sizeof(buffer)); // copy over contents to the return buffer
	return ret;
}

/**
 * Purpose: To take a value (hex string) and check if it is valid to parse. If so, parse
 * the hex digit from the string and check to see if the value has a key in the hash map.
 * If so return the string key associated, else inform the user it is not a valid value.
 * This function decodes a hexidecimal encoded tweet of 16 bits length.
 */
char *decode(char *msg, char **hashTable) {
	int bufferSize = (MAX_TWEET*20)+1; // assuming the max length of 140 character word and max number than can fit for checking of 20
	char *ret, *words[MAX_TWEET], *token;
	memset(words, 0, sizeof(MAX_TWEET)); // zero out the memory for words

	// allocate memory for return pointer
	if ((ret = malloc(sizeof(char)*bufferSize)) == NULL) {
		fprintf(stderr, "Error allocating memory\n");
		exit(EXIT_FAILURE);
	}

	// split into tokens based on " "
	token = strtok(msg, " ");
	int size = 0;
	while (token != NULL) {
		words[size] = token;
		token = strtok(NULL, " ");
		size++;
	}

	// check to make sure each word is prefixed properly
	int returnVal;
	char buffer[bufferSize];
	memset(buffer, 0, bufferSize);
	// messagesfor if hex value is no good (is added in as if it were the hex value)
	char errMsg1[] = "(Not in table) ";
	char errMsg2[] = "(Invalid) ";

	// decrypt each hex word
	for (int i = 0; i < size; i++) {
		char *prefix = strstr(words[i], "0x");
		if ((prefix-words[i]) != 0) { // check the prefix to see if it is correct
			strcat(buffer, errMsg2);
		}
		else { // if the prexif is correct
			int value;
			int checkNums = 0;
			// check to make sure all the characters are the right type
			if (strlen(words[i]) > 6) {
				strcat(buffer, errMsg2);
				continue;
			}
			// check each character to make sure it is a digit or between a-f (in both cases)
			for (int j = 2; j < strlen(words[i]); j++) {
				if (isdigit(words[i][j]) || ((tolower(words[i][j]) >= 97) && (tolower(words[i][j]) <= 102))) {
					continue; // if it is a valid character keep checking
				}
				else {
					checkNums = -1; // if invalid, exit the loop to process error
					break;
				}
			}
			// if character is invalid, inform the user
			if (checkNums < 0) {
				strcat(buffer, errMsg2);
				continue; // go to the next word since this one is invalid
			}
			else {
				// convert string to hex number
				returnVal = sscanf(words[i], "0x%04x", &value);
				if ((returnVal == 0) || (returnVal == EOF)) {
					strcat(buffer, errMsg2);
					continue; // go to the next word since this one is invalid
				}
				else {
					// if it makes it here than it is a valid hex number, so get its key
					char *key = searchValue(value, hashTable);
					if (key == NULL) strcat(buffer, errMsg1);
					else {
						strcat(buffer, key); // copy key to temp buffer for returning to client
						strcat(buffer, " ");
					}
				}
			}
		}
	}
	strncpy(ret, buffer, bufferSize); // copy buffer over to be returned
	return ret;
}



