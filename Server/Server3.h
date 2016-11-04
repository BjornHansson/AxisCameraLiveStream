#ifndef _Server3_H_ /* Include guard */
#define _Server3_H_

#include <arpa/inet.h> // e.g. Host Address Functions https://www.gnu.org/software/libc/manual/html_mono/libc.html#Host-Address-Functions
#include <capture.h> // Axis Capture Interface, for reading images file:///home/axis/Desktop/axis/acap/docs/html/capture_8h.html
#include <crypt.h> // e.g. Encryption https://www.gnu.org/software/libc/manual/html_mono/libc.html#DES-Encryption
#include <errno.h> // e.g. Error Messages https://www.gnu.org/software/libc/manual/html_mono/libc.html#Error-Messages
#include <netdb.h> // e.g. sockets https://www.gnu.org/software/libc/manual/html_mono/libc.html#Sockets
#include <netinet/in.h> // e.g. Internet namespace https://www.gnu.org/software/libc/manual/html_mono/libc.html#Internet-Namespace
#include <stdio.h> // Input/Output on Streams https://www.gnu.org/software/libc/manual/html_mono/libc.html#I_002fO-on-Streams
#include <stdlib.h> // e.g. Basic Memory Allocation https://www.gnu.org/software/libc/manual/html_mono/libc.html#Basic-Allocation
#include <string.h> // e.g. string processing utilities https://www.gnu.org/software/libc/manual/html_mono/libc.html#String-and-Array-Utilities
#include <syslog.h> // e.g. Syslog Messages https://www.gnu.org/software/libc/manual/html_mono/libc.html#Syslog 
#include <sys/time.h> // e.g. High-Resolution Calendar https://www.gnu.org/software/libc/manual/html_mono/libc.html#High_002dResolution-Calendar
#include <sys/socket.h> // e.g. Sockets https://www.gnu.org/software/libc/manual/html_mono/libc.html#Sockets
#include <sys/types.h> // e.g. Waiting for Input or Output https://www.gnu.org/software/libc/manual/html_mono/libc.html#Waiting-for-I_002fO
#include <sys/stat.h> // e.g. Setting Permissions file attributes https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-Permissions
#include <time.h> // e.g. Simple Calendar Time. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Simple-Calendar-Time
#include <unistd.h> // e.g. Input and Output Primitives https://www.gnu.org/software/libc/manual/html_mono/libc.html#I_002fO-Primitives
#include <math.h>

void initRSA();
int prime(int pr);
int isprime(int n);
int compute_eCoprime(int t);
int compute_dModularMultiplicativeInverse(int e, int t);
int encryptRSA(int m, int n, int e);
int decryptRSA(int mEnc, int d, int n);
int randPrime();
int readIntS3(int aSocketDescriptor);
char* concat(char* s1, char* s2);
char* intToStrResolution(int r);
void initializeSocketListener();
void acceptConnectionFromClient();
void readDataFromClient();
void sendDataToClient();
void setXORKey(int XORKey);

#endif
