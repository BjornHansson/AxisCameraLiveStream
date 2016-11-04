/*
 2016-10-27 21:00

 ACAP Server for Axis camera.
 */

#include "Server3.h"

media_frame* pointerToMediaFrame;
media_stream* pointerToMediaStream;
int socketDescriptor, newSocketDescriptor;
const int PORT_NUMBER = 5005;
struct sockaddr_in serverSockAddr, clientSockAddr;
int processIdentifier;
char* mediaProperties;
char* CAPTURE_CAMERA_IP = "127.0.0.1";
signed char XORByteKeysArray[2];
int XORKeyInt = 0;
int password = 1234;
int pFirstPrime = 0, qSecondPrime = 0, nPrimeProduct = 0, tTotientPrimeProduct =
		0;
int eCoprime = 0, dModularMultiplicativeInverse = 0;

int main(int argc, char** argv) {
	initializeSocketListener();
	// Forever loop
	while (1) {
		acceptConnectionFromClient();
		if (processIdentifier == 0) {
			readDataFromClient();
			while (1) {
				sendDataToClient();
			}
		} else {
			close(newSocketDescriptor);
		}
	}
}

char* concat(char* char1, char* char2) {
	// malloc function returns a pointer to the allocated memory, or NULL if the request fails.
	char *result = malloc(strlen(char1) + strlen(char2) + 1); //string length + one for the extra '\0'
	strcpy(result, char1); // strcpy returns a pointer to the destination string dest.
	strcat(result, char2);
	return result;
}

char* intToStrResolution(int r) {
	switch (r) {
	case 0:
		return "320x180";
	case 1:
		return "480x270";
	case 2:
		return "640x360";
	case 3:
		return "800x450";

	}
	return "320x180";
}

void initializeSocketListener() {
	// First call to socket() function
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor < 0) {
		syslog(LOG_INFO, "Error opening socket");
		exit(1);
	}
	// Initialize socket structure
	bzero((char *) &serverSockAddr, sizeof(serverSockAddr));
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_addr.s_addr = INADDR_ANY;
	serverSockAddr.sin_port = htons(PORT_NUMBER);
	// Now bind the host address using bind() call.
	if (bind(socketDescriptor, (struct sockaddr *) &serverSockAddr,
			sizeof(serverSockAddr)) < 0) {
		syslog(LOG_INFO, "Error on binding");
		exit(1);
	}
	// Print message on log with info level. http://www.gnu.org/software/libc/manual/html_node/Syslog.html#Syslog
	// Now start listening for the clients, here process will go in sleep mode and will wait for the incoming connection.
	// http://www.gnu.org/software/libc/manual/html_node/Listening.html
	listen(socketDescriptor, 5);
}

void acceptConnectionFromClient() {
	// Accept actual connection from the client when server receives a connection request. http://www.gnu.org/software/libc/manual/html_node/Accepting-Connections.html#Accepting-Connections
	socklen_t clientSockAddrLength = sizeof(clientSockAddr);
	newSocketDescriptor = accept(socketDescriptor,
			(struct sockaddr *) &clientSockAddr, &clientSockAddrLength);
	if (newSocketDescriptor < 0) {
		syslog(LOG_INFO, "Error on accept");
		exit(1);
	} else {
		syslog(LOG_INFO, "Accepted connection from the client");
		// Create child process. http://www.gnu.org/software/libc/manual/html_node/Creating-a-Process.html
		processIdentifier = fork();
		if (processIdentifier < 0) {
			syslog(LOG_INFO, "Error on fork");
			exit(1);
		}
	}
}

void readDataFromClient() {
	// This is the client process
	unsigned char resolution = 0;
	unsigned char framesPerSecond = 0;
	initRSA();
	//syslog(LOG_INFO, "pFirstPrime %d", pFirstPrime);
	//syslog(LOG_INFO, "qSecondPrime %d", qSecondPrime);
	// eCoprime
	//syslog(LOG_INFO, "eCoprime %d", eCoprime);
	int htonleCoprime = htonl(eCoprime);
	write(newSocketDescriptor, (void *) &htonleCoprime, sizeof(eCoprime));
	// nPrimeProduct
	//syslog(LOG_INFO, "nPrimeProduct %d", nPrimeProduct);
	int htonlnPrimeProduct = (htonl(nPrimeProduct));
	write(newSocketDescriptor, (void *) &htonlnPrimeProduct,
			sizeof(nPrimeProduct));
	// info dModularMultiplicativeInverse
	//syslog(LOG_INFO, "dModularMultiplicativeInverse %d", dModularMultiplicativeInverse);
	// password
	int encryptedpasswordFromClient = readIntS3(newSocketDescriptor);
	//syslog(LOG_INFO, "encryptedpasswordFromClient %d", encryptedpasswordFromClient);
	int decryptedPasswordFromClient = decryptRSA(encryptedpasswordFromClient,
			dModularMultiplicativeInverse, nPrimeProduct);
	//syslog(LOG_INFO, "decryptedPasswordFromClient %d", decryptedPasswordFromClient);
	if (decryptedPasswordFromClient == password) {
		//syslog(LOG_INFO, "After passwort check");
		setXORKey(
				decryptRSA(readIntS3(newSocketDescriptor),
						dModularMultiplicativeInverse, nPrimeProduct));
		//syslog(LOG_INFO, "XORKeyInt %d", XORKeyInt);
		//syslog(LOG_INFO, "XORByteKeysArray[0] %d", XORByteKeysArray[0]);
		//syslog(LOG_INFO, "XORByteKeysArray[1] %d", XORByteKeysArray[1]);
		resolution = decryptRSA(readIntS3(newSocketDescriptor),
				dModularMultiplicativeInverse, nPrimeProduct);
		framesPerSecond = decryptRSA(readIntS3(newSocketDescriptor),
				dModularMultiplicativeInverse, nPrimeProduct);
		char* resolutionStr = intToStrResolution(resolution);
		char fpsStr[3];
		sprintf(fpsStr, "%d", framesPerSecond);
		// mediaProperties see file:///home/axis/Desktop/axis/acap/docs/html/vidcap_8c-example.html#a2
		char* tempMediaProp1 = concat("capture-cameraIP=", CAPTURE_CAMERA_IP);
		char* tempMediaProp2 = concat(tempMediaProp1,
				"&capture-userpass=root:pass&resolution=");
		char* tempMediaProp3 = concat(tempMediaProp2, resolutionStr);
		char* tempMediaProp4 = concat(tempMediaProp3, "&fps=");
		mediaProperties = concat(tempMediaProp4, fpsStr);
		// Print message on logo
		//syslog(LOG_INFO, "mediaProperties %s", mediaProperties);
		// Opens a new stream of the specified media type with the specified properties.
		// file:///home/axis/Desktop/axis/acap/docs/html/capture_8h.html#a0c8455627728c4bb5194632b8813a67c
		pointerToMediaStream = capture_open_stream(IMAGE_JPEG, mediaProperties);
		free(tempMediaProp1);
		free(tempMediaProp2);
		free(tempMediaProp3);
		free(tempMediaProp4);
		close(socketDescriptor);
	} else {
		close(socketDescriptor);
		exit(0);
	}
}

void sendDataToClient() {
	void* pointerToCaptureFrameData;
	// Read a media_frame from an open stream.
	// file:///home/axis/Desktop/axis/acap/docs/html/capture_8h.html#a64e6daae3c3a8c2b0f8809e32891d58e
	pointerToMediaFrame = capture_get_frame(pointerToMediaStream); // capture_get_frame is blocking
	// Obtain the data size from the media_frame.
	// file:///home/axis/Desktop/axis/acap/docs/html/capture_8h.html#af23dafedfa149bd822e65c99660e91ce
	size_t picDataSize = capture_frame_size(pointerToMediaFrame);
	//syslog(LOG_INFO, "pic data size %d", picDataSize);
	int picDataSizeXOR = ((int) picDataSize) ^ XORKeyInt;
	int datasizeint = htonl((int) picDataSizeXOR);
	write(newSocketDescriptor, (void *) &datasizeint, sizeof(datasizeint));
	// file:///home/axis/Desktop/axis/acap/docs/html/capture_8h.html#a8e894677776d3c87fe6b89c50bb672a8
	// Obtain the pointerToCaptureFrameData from the media_frame.
	pointerToCaptureFrameData = capture_frame_data(pointerToMediaFrame);
	int i;
	unsigned char encryptedPicBuffer[picDataSize];
	int keyLength = sizeof(XORByteKeysArray) / sizeof(XORByteKeysArray[0]);
	for (i = 0; i < picDataSize; i++) {
		encryptedPicBuffer[i] = ((unsigned char*) pointerToCaptureFrameData)[i]
				^ XORByteKeysArray[(keyLength - 1) - (i % keyLength)];
	}
	int bytesWritten = write(newSocketDescriptor, &encryptedPicBuffer[0],
			picDataSize);
	if (bytesWritten == -1) {
		syslog(LOG_INFO, "send pic faild exiting this child process");
		exit(0);
	}
	// Free the media_frame retrieved from capture_get_frame().
	// file:///home/axis/Desktop/axis/acap/docs/html/capture_8h.html#a2f638703b12683a34174f2e745ee4726
	capture_frame_free(pointerToMediaFrame);
	//syslog(LOG_INFO, "mediaProperties %s", mediaProperties);
}

void initRSA() {
	srand(time(0));
	pFirstPrime = randPrime();
	qSecondPrime = randPrime();
	nPrimeProduct = pFirstPrime * qSecondPrime;
	tTotientPrimeProduct = (pFirstPrime - 1) * (qSecondPrime - 1);
	eCoprime = compute_eCoprime(tTotientPrimeProduct);
	dModularMultiplicativeInverse = compute_dModularMultiplicativeInverse(
			eCoprime, tTotientPrimeProduct);
}

int compute_eCoprime(int t) {
	int i = 0;
	for (i = 2; i < t; i++) {
		if (isprime(i) == 0) {
			continue; // not a prime
		}
		if (t % i == 0) {
			continue; // not a coprime
		}
		break;
	}
	return i; // return the coprime
}

int compute_dModularMultiplicativeInverse(int e, int t) {
	int returnTemp = 0;
	int nt = 1;
	int r = t;
	if (t < 0) {
		t = -t;
	}
	if (e < 0) {
		e = t - (-e % t);
	}
	int nr = e % t;
	while (nr != 0) {
		int quot = (r / nr) | 0;
		int tmp = nt;
		nt = returnTemp - quot * nt;
		returnTemp = tmp;
		tmp = nr;
		nr = r - quot * nr;
		r = tmp;
	}
	if (r > 1) {
		return -1;
	}
	if (returnTemp < 0) {
		returnTemp = returnTemp + t;
	}
	return returnTemp;
}

int encryptRSA(int m, int n, int e) {
	int j = 0;
	long long int encryptedM = 1;
	for (j = 0; j < e; j++) {
		encryptedM = encryptedM * m % n;
	}
	return (int) encryptedM;
}

int decryptRSA(int mEnc, int d, int n) {
	int j = 0;
	long long int decryptedM = 1;
	for (j = 0; j < d; j++) {
		decryptedM = decryptedM * mEnc % n;
	}
	return (int) decryptedM;
}

int isprime(int n) {
	int i;
	if (n % 2 == 0 || n == 1) {
		return 0;
	}
	for (i = 3; i * i <= n; i += 2) {
		if (n % i == 0) {
			return 0;
		}
	}
	return 1;
}

int randPrime() {
	int r;
	while (!isprime((r = (rand() % 1000 + 260))));
	return r;
}

int readIntS3(int aSocketDescriptor) {
	unsigned char buf[4];
	int k = 0;
	while (1) {
		int nbytes = recv(newSocketDescriptor, &buf[k], 1, 0);
		if (nbytes == -1) {
			break;
		}
		if (nbytes == 0) {
			break;
		}
		if (k == 3) {
			break;
		}
		k++;
	}

	unsigned long int i2 = ((unsigned long int) buf[0] << 24)
			| ((unsigned long int) buf[1] << 16)
			| ((unsigned long int) buf[2] << 8) | buf[3];
	int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffffffu) {
		i = i2;
	} else {
		i = -1 - (long int) (0xffffffffu - i2);
	}
	return i;
}

void setXORKey(int XORKey) {
	XORKeyInt = XORKey;
	XORByteKeysArray[0] = (signed char) (XORKey & 0xFF);
	XORByteKeysArray[1] = (signed char) ((XORKey >> 8 & 0xFF));
}
