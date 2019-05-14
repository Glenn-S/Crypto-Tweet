-------------------------------------------------------------------------------------------------------
CPSC 441 
Assignment 2
Glenn Skelton

-------------------------------------------------------------------------------------------------------
CONTENTS:

makefile
artifacts
hashTests
	test1.c
src
	cryptoTweetClient.c
	cryptoTweetServer.c
	definitions.h
	hash.c
	hash.h
	process.c
	process.h

-------------------------------------------------------------------------------------------------------
COMPILE:

To compile cryptoTweet there is included a makefile that has the following commands:
(all object files are stored in artifacts folder, source files are in src folder)

make all - builds all object files and executables. the executables are 
	   named client and server respectively

-- additional commands --
make cryptoTweetServer.o - builds object file for server
make cryptoTweetClient.o - builds object file for client
make process.o - builds object file for process
make hash.o - builds object file for hash
make server - builds the object files, links and compiles the server into executable named server
make client - builds the object files, links and compiles the client into executable named client
make clean - removes all object files and executables 

To compile manually type while in the src file:
	>>>gcc -Wall -g <cryptoTweetServer.c/cryptoTweetClient.c> -o <name of executable>

To run the server program:
	>>>./server -[s, w, m] (these are command line options to choose the different hashing scheme
	>>>Please enter an IP address: <eg. 127.0.0.1 format>
	
	-s sequential index scheme
	-w word sum index scheme
	-m my implementation scheme

To run the client program:
	>>>./client
	>>>Please enter an IP address: <eg. 127.0.0.1 format>

If you wish to run in debug mode, there is a toggle constant called DEBUG in the definitions.h that 
when set to 1 will turn on debug mode. There is also a single user mode which can be toggled by
changing SINGLE (in definitions.h) from 0 to 1.

As the client you can enter or search for tweets by either writting your sentence (must be less than
or equal to 140 characters) or writting hexadecimal numbers of the form 0x____. When entering a 
sentence, the words will be hashed and the encrypted form will be returned to the client. This program
will accept sentence with punctuation however the user likes and return the encrypted form with the 
punctuation in the same location. If the client enters a word(s) for decryption, the program will
return the associated word(s). If the hash is not in the table, a message will show up where the word 
was written along with other successful or not successful words. The program will continue waiting for
the client until the client enters "-EXIT".

-------------------------------------------------------------------------------------------------------
DESIGN:

In my program I chose to use TCP because I felt that, in an application like this, it was important to
have the benefits of TCP's reliable data transportation. Since the tweets are not very long, I felt 
that the trade off in speed was negligable.  My need for having the message show upreliability was 
much greater. I also decided that I wanted to have my application be session oriented because I figured
that a user wouldn't just encrypt/decrypt one word at a time. Since TCP has to go through the 3-way
handshake every time it establishes a new connection, it made no sense to close the connection after 
every word since this would just make the program much slower. I also chose TCP because reduced the
complexity of my program. This choice allowed me to write much more concise code and as a result the 
length was much shorter than what I imagine it would have been if I had written it in TCP. Since TCP
is implemented at the OS scope, I feel my application will be much more portable since most every 
computer has the requirement to implement TCP. 

For my hashing scheme I decided to make a fixed table of 65535 since there are so many repeated words
in the English language. For dealing with collisions, I chose to user quadratic probing to try and space out the table and hopefully reduce the number of collisions. My personal hashing algorithm implements a
variation on the sum hash version however, in order to reduce collision of  words like anagrams, I used
a version that multiplies the characters ASCII value by its byte position in the string. The indexing
is in high byte order down (reverse indexing). The value calculated is then added together to get the
strings 16 bit value for hashing. choose this algorithm because I knew that it would take care of the 
anagrams issue from the word sum algorithm. In order to validate my choice, I created a test that used
the 100 most common English words and ran their hashes on both algorithms. Where word sum had 29 words 
of the 100 hash to the same value, my choice only had 11 collisions. The issue with this algorithm 
however is that words can still create sums that are equal to others (like as and he) due to the fact
that it is still working from a fixed mathematical operation that relies on the ASCII values. 

As far as other choice, I wanted to make the application as portable as possible so instead of statically
setting the IP address, I allow the server to choose their own. As a result the client must also input 
an address up startup. I see this as no different than having to enter a URL in http though which I felt
justified this choice. I did however choose to set the PORT number because I felt a regular user would
not feel comfortable working with port numbers and it also requires the user to have that extra piece 
of information. I decided to keep it constant like HTTP is with port 80. I chose the port 12345 
arbitrarily because it was free to use. I also chose to make the client application much more forgiving
to the user by handling exceptions gracefully and keeping the application going unless there is an error
that is fatal. 

-------------------------------------------------------------------------------------------------------
TESTING:

For testing I initially tested the application on a linux virtual machine on the internal loopback IP 
127.0.0.1 to work out the major testing on my personal coputer. Once the application was functioning I 
performed my testing across the U of C network using a Mac and a Linux virtual machine. As far as I can
tell, the application responds how I expect it too based on the requirements. The only thing I wish I 
could have implemented was an interrupt system for the multi user mode (not multi threaded) so that I
can shut it off without having to use CTRL^C or another terminal to kill the process. As far as I can 
tell from my testing, the hashing works as expected for all three types. 

-------------------------------------------------------------------------------------------------------
NOTES:

I did not successfully complete the bonus.
-------------------------------------------------------------------------------------------------------

