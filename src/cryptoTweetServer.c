/**
 * Crypto Tweet cryptoTweetServer.c
 *
 * CPSC 441, Assignment 2
 * Glenn Skelton
 *
 * a tweet has only 140 character (140 bytes)
 * I have chosen the connection-oriented TCP protocol
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <strings.h>
#include <netdb.h>

#include "process.h"
#include "definitions.h" // to toggle debugging, see definitions.h

// define global variable (defined exactly once)
int seqHashVal = 0;
int ENCRYPT = 0;


/**
 * Purpose: To act as the server for the crypto tweet application. It will take a user
 * entry and either decrypt or encrypt based on the format of the entry.
 */
int main(int argc, char *argv[]) {
	char IP_ADDR[IP_BUFF]; // array for storing IP address

	char **hashTable; // hash map array
	if ((hashTable = malloc(sizeof(char*)*MAX_VAL)) == NULL) {
		fprintf(stderr, "Error allocating memory\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < MAX_VAL; i++) hashTable[i] = NULL; // initialize all elements to null

	// get the type of encryption to use and store the state for the duration of the session
	if (argc != 2) {
		printf("./cryptoTweetServer <Encyption type(-s -w -m)>\n");
		free(hashTable); // deallocate memory for hash map
		exit(EXIT_FAILURE);
	}
	// set the encryption type
	if (strcmp(argv[1], "-s") == 0) ENCRYPT = 1; // sequential
	else if (strcmp(argv[1], "-w") == 0) ENCRYPT = 2; // wordCount
	else if (strcmp(argv[1], "-m") == 0) ENCRYPT = 3; // my own
	else {
		printf("Invalid command line option, (-s -w -m)\n");
		free(hashTable); // deallocate memory for hash map
		exit(EXIT_FAILURE);
	}

	// get the IP address to use
	printf("Please enter an IP address: ");
	int IP1, IP2, IP3, IP4;
	int retVal = scanf("%d.%d.%d.%d", &IP1, &IP2, &IP3, &IP4);
	if ((retVal < 4) || (IP1 > 255) || (IP2 > 255) || (IP3 > 255) || (IP4 > 255)) {
		fprintf(stderr, "IP read in failed %d\n", retVal);
		free(hashTable); // deallocate memory for hash map
		exit(EXIT_FAILURE);
	}
	// set IP address based on user input
	memset(IP_ADDR, 0, sizeof(char)*IP_BUFF);
	sprintf(IP_ADDR, "%d.%d.%d.%d", IP1, IP2, IP3, IP4);
	printf("IP : %s\n", IP_ADDR);
	// flush the stdin buffer
	char ch;
	while ((ch = getchar()) != '\n') {
		continue;
	}


#if DEBUG
	printf("Option was ENCRYPT %d\n", ENCRYPT);
#endif

	// get server socket ready
	struct sockaddr_in server;
	int server_desc;
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_addr.s_addr = inet_addr(IP_ADDR); // set up for specified IP // or could use htonl(INADDR_ANY)
	server.sin_family = AF_INET; // setup for IPv4
	server.sin_port = htons(PORTNUM);

	// create server socket
	if ((server_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket creation failed\n");
		free(hashTable); // deallocate memory for hash map
		exit(EXIT_FAILURE);
	}

	// bind the socket
	if (bind(server_desc, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr, "Binding server socket failed\n");
		close(server_desc);
		free(hashTable); // deallocate memory for hash map
		exit(EXIT_FAILURE);
	}

	// set socket to listen
	if (listen(server_desc, 10) < 0) {
		fprintf(stderr, "Listen for server failed\n");
		close(server_desc);
		free(hashTable);
		exit(EXIT_FAILURE);
	}
	printf("Listening for requests...\n");

	/** END OF SETUP **/

	// main interaction loop
	// the server is always on
	while (1) {
		// accept connection
		int client_desc;
		if ((client_desc = accept(server_desc, NULL, NULL)) < 0) {
			fprintf(stderr, "Error accepting the client\n");
			close(server_desc);
			free(hashTable); // deallocate memory for hash map
			exit(EXIT_FAILURE);
		}
		printf("Connection established with client\n");

#if DEBUG
		printf("Accepted connection\n");
#endif

		char buffer[MAX_TWEET+1]; // room for buffer

		// get messages from client and respond loop
		int connected = 1;
		while (connected) {
			// get the users message
			int rcvCode;
			memset(buffer, 0, sizeof(buffer));

			// Receive data
			if ((rcvCode = recv(client_desc, buffer, sizeof(buffer), 0)) < 0) {
				printf("Error receiving message\n");
			}
#if DEBUG
			if (rcvCode > 0) printf("client: %s\n", buffer); // need to buffer to get the full message
#endif
			if (strstr(buffer, "-EXIT") != NULL) {
				connected = 0; // terminate the connection loop
			}
			else {
				// take care of the encoding or decoding
				char response[1000];
				memset(response, 0, sizeof(char)*1000);

				// send the user their message encoded or decoded back
				char *tempStorage = parseString(buffer, hashTable);
				if (tempStorage == NULL) {
					break; // there is nothing to send back to the user so go to getting another message
				}
				sprintf(response, "%s\n", tempStorage);
				free(tempStorage); //free up return pointer memory
				// send the client the encrypted/decrypted message
				if (send(client_desc, response, sizeof(response), 0) < 0) {
					fprintf(stderr, "Error in sending response to client\n");
					close(client_desc);
					free(hashTable); // deallocate memory for hash map
					exit(EXIT_FAILURE);
				}
			}
		}
		// close the socket if terminated
		close(client_desc);
		printf("Connection terminated\n");
#if SINGLE
		break; // shutdown the program after client enters exit for debug
#endif
	}
	close(server_desc);
	printf("Server shutdown...\n");

	// free memory
	for (int i = 0; i < MAX_VAL; i++) {
		free(hashTable[i]);
	}
	free(hashTable);
	return 0;
}





