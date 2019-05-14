/**
 * Filename: test1.c
 *
 * Assignment 2
 * CPSC 441
 * Glenn Skelton
 *
 * Test file for studying the hash schemes effectiveness
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Testing for hashing algorithms idempotence
 */

int wordSumHash(char *key) {
	// search through and get the letters of the word
	int sum = 0, i = 0;
	while (key[i] != '\0') {
		sum += key[i++]; // get the character's ascii value and add it to sum
	}
	return sum;
}

int myHash(char *key) {
	int i = 0, total = 0;
	unsigned short tmp; // tmp is 2 bytes
	int n = strlen(key);

	while (key[i] != '\0') {
		tmp = key[i]*(n-i); // multiply the value of each character by its high order position
		//total += (tmp << 8) + (tmp >> 8); // sum the high and low order bytes
		total += tmp;
		i++;
	}
	return total;
}

int main() {
	// 100 most common English words
	char *words[100] = {
		"the","be","to","of","and","a","in","that","have","I","it","for","not","on","with","he","as",
		"you","do","at","this","but","his","by","from","they","we","say","her","she","or","an","will",
		"my","one","all","would","there","their","what","so","up","out","about","who","get","which",
		"go","me","when","make","can","like","time","no","just","him","know","take","people","into",
		"year","your","good","some","could","them","see","other","than","then","now","look","only",
		"come","its","over","think","also","back","after","use","two","how","our","work","first","well",
		"way","even","new","want","because","any","these","give","day","most","us"};

	// get words hashes
	int wordNums[100];
	for (int i = 0; i < 99; i++) {
		wordNums[i] = myHash(words[i]);
		printf("index = %d, %s = 0x%04x\n", i, words[i], myHash(words[i]));
		//printf("%s\n", words[i]);
	}

	// evaluate if any hashes are the same
	printf("Checking the list\n");
	int total = 0;
	int bool = 0;
	for (int i = 0; i < 99; i++) {
		for (int j = i+1; j < 99; j++) {
			if (wordNums[i] == wordNums[j]) {
				bool = 1;
				printf("Match, index = %d, %d; words %s == %s\n", i, j, words[i], words[j]);
				total++;
			}
		}
	}
	if (bool == 1) {
		printf("Hash produce two of the same hashing\n");
	}
	printf("Total matches: %d\n", total);



	// test sum hash
	total = 0;
	for (int i = 0; i < 99; i++) {
		wordNums[i] = wordSumHash(words[i]);
		printf("index = %d, %s = 0x%04x\n", i, words[i], wordSumHash(words[i]));
		//printf("%s\n", words[i]);
	}
	printf("Checking the list\n");
	bool = 0;
	for (int i = 0; i < 99; i++) {
		for (int j = i+1; j < 99; j++) {
			if (wordNums[i] == wordNums[j]) {
				bool = 1;
				printf("Match, index = %d, %d; words %s == %s\n", i, j, words[i], words[j]);
				total++;
			}
		}
	}
	if (bool == 1) {
		printf("Hash produce two of the same hashing\n");
	}
	printf("Total matches: %d\n", total);

	return 0;
}
