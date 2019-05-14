/**
 * Crypto Tweet cryptoTweeetClient.c
 *
 * CPSC 441, Assignment 2
 * Glenn Skelton
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

#include "definitions.h" // to toggle debugging, see definitions.h


/**
 * Purpose: To act as the client in the crypto tweet application. It will connect to
 * a server which will process the clients tweet and either encrypt or decrypt based on
 * the users input. The database will store the encryption of each word in a hash lookup
 * table.
 */
int main(int argc, char *argv[]) {
	char IP_ADDR[IP_BUFF], ch; // array for storing IP address

	// get the IP address to connect with
	printf("Please enter an IP address to connect to: ");
	int IP1, IP2, IP3, IP4;
	int retVal = scanf("%d.%d.%d.%d", &IP1, &IP2, &IP3, &IP4);
	// check to make sure all of the values of the address are correct
	if ((retVal < 4) || (IP1 > 255) || (IP2 > 255) || (IP3 > 255) || (IP4 > 255)) {
		fprintf(stderr, "IP read in failed %d\n", retVal);
		exit(EXIT_FAILURE);
	}

	// set IP address based on user input
	memset(IP_ADDR, 0, sizeof(char)*IP_BUFF);
	sprintf(IP_ADDR, "%d.%d.%d.%d", IP1, IP2, IP3, IP4);
#if DEBUG
	printf("IP : %s\n", IP_ADDR);
#endif
	// flush the stdin buffer
	while ((ch = getchar()) != '\n') {
		continue;
	}

	// set up socket
	struct sockaddr_in client;
	memset(&client, 0, sizeof(struct sockaddr_in));
	int client_desc;
	client.sin_addr.s_addr = inet_addr(IP_ADDR);
	client.sin_family = AF_INET;
	client.sin_port = htons(PORTNUM);

	// create client socket
	if ((client_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error creating client socket\n");
		exit(EXIT_FAILURE);
	}
#if DEBUG
	printf("Client socket created\n");
#endif
	// connect client to server (initiate TCP handshake)
	if (connect(client_desc, (struct sockaddr*)&client, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr, "Connection with server failed\n");
		close(client_desc);
		exit(EXIT_FAILURE);
	}
#if DEBUG
	printf("Connection established\n");
#endif
	/** END OF SETUP **/

	printf("\n\nWelcome to Crypto Tweet!\n\n");
	printf("Type a message to encrypt it or enter an encrypted tweet\n");
	printf("to decrypt it (must begin of 0x and be max 4 digits wide)\n" \
		   "or type \"-EXIT\" to leave\n\n");

	// main program loop
	int connected = 1;
	while (connected) {
		char message[MAX_TWEET+1];
		memset(message, 0, sizeof(message));

		// need to limit the users message to 140 characters
		ch = getchar(); // read the character in
		int index = 0;
		while ((ch != EOF) && (ch != '\n')) {
			if (index < MAX_TWEET) {
				message[index++] = ch;
			}
			else {
				while ((ch = getchar()) != '\n') {
					continue; // get rid of the input file buffer characters
				}
				index = -1; // to signifiy that the tweet was too long
				break;
			}
			ch = getchar();
		}

		if (index < 0) printf("Tweet is too long, you may only use 140 characters\n");
		else {
			if (send(client_desc, message, sizeof(message), 0) < 0) {
				fprintf(stderr, "Error sending message to server\n");
				close(client_desc);
				exit(EXIT_FAILURE);
			}
			if (strstr(message, "-EXIT") != NULL) {
				connected = 0; // terminate connection while loop
				continue;
			}
			// recieve the response back from the server
			char reply[1000];
			memset(reply, 0, sizeof(reply));
			if (recv(client_desc, reply, sizeof(reply), 0) < 0) {
				printf("Reply failure\n");
			}
			printf("%s\n", reply); // print the servers reply out
		}
	}
	// terminate client program
	printf("Goodbye!!\n");
	close(client_desc);
	return 0;
}




