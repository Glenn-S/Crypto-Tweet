/**
 * Crypto Tweet hash.h
 *
 * CPSC 441, Assignment 2
 * Glenn Skelton
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HASH_H
#define HASH_H

/* HASH TABLE FUNCTIONS */
void printHashTable(char**);
int search(char*, char**);
char *searchValue(int val, char**);
int insert(char*, int (*f)(char*), char**);

/* HASH ALGORITHMS */
int seqHash(char*);
int wordSumHash(char*);
int myHash(char*);

#endif
