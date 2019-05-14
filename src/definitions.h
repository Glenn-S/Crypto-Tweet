/**
 * Crypto Tweet definitions.h
 *
 * CPSC 441, Assignment 2
 * Glenn Skelton
 *
 */

#include <stdio.h>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* DEBUGGING */
#define DEBUG 0 // 1 to turn on 0 to turn off
#define SINGLE 1 // 1 to turn on single client mode and 0 to turn off


/* CONSTANTS */
#define PORTNUM 12345
#define IP_BUFF 16
#define MAX_TWEET 140
#define MAX_VAL 65535

/* GLOBAL VARIABLES */
extern int ENCRYPT;
extern int seqHashVal;

#endif
