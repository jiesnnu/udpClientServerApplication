/*******************************
udp_client.c: the source file of the client in udp
********************************/

#include "headsock.h"

float transmitPackets(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, long *len);                
void getTimeInterval(struct  timeval *out, struct timeval *in);

int main(int argc, char *argv[])
{
	long len;
	int sockfd;
	struct sockaddr_in ser_addr;
	char **pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	float rateOfTransmission;
	float timeInterval;
	FILE* filePointer;

	if (argc!= 2)
	{
		printf("parameters not match.");
		exit(EXIT_FAILURE);
	}

	if ((sh=gethostbyname(argv[1]))==NULL) {             
		printf("error when gethostbyname");
		exit(EXIT_FAILURE);
	}
	//Create Socket for Operation
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);             
	if (sockfd < 0)
	{
		printf("Error in Creating Socket");
		exit(EXIT_FAILURE);
	}

	addrs = (struct in_addr **)sh->h_addr_list;
	printf("canonical name: %s\n", sh->h_name);
	//Print out socket information
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);			
	switch(sh->h_addrtype)
	{
		case AF_INET:
			printf("AF_INET\n");
		break;
		fileSfault:
			printf("unknown addrtype\n");
		break;
	}
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MYUDP_PORT);
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);
	if((filePointer = fopen ("myfile.txt","r+t")) == NULL)
	{
		printf("File doesn't exist\n");
		exit(EXIT_FAILURE);
	}
	timeInterval = transmitPackets(filePointer, sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in), &len);   
	rateOfTransmission = (len/(float)timeInterval);
	printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", timeInterval, (int)len, rateOfTransmission);
	close(sockfd);
	exit(EXIT_SUCCESS);
}

float transmitPackets(FILE *fp, int sockfd, struct sockaddr *serverAddress, int addrlen, long *len)
{
	char *buf;
	long fileSize, totalSizeSent;
	char sends[DATALEN];
	int socketLength = sizeof(struct sockaddr_in);
	struct ack_so acknowledgement;
	//Packet length + Header Length = totalPacketLength
	int n, totalPacketLength;
	float timeInterval = 0.0;
	struct timeval timeOfSending, timeOfReceiving;
	totalSizeSent = 0;

	
	fseek (fp , 0 , SEEK_END);
	fileSize = ftell (fp);
	rewind (fp);
	printf("The file length is %d bytes\n", (int)fileSize);
	printf("the packet length is %d bytes\n",DATALEN);

	// allocate memory to contain the whole file.
	buf = (char *) malloc (fileSize);
	if (buf == NULL) exit (EXIT_FAILURE);

  	// copy the file into the buffer.
	fread (buf,1,fileSize,fp);

  	/*** the whole file is loaded in the buffer. ***/
	buf[fileSize] ='\0';									
	gettimeofday(&timeOfSending, NULL);							
	while(totalSizeSent<= fileSize)
	{
		if ((fileSize+1-totalSizeSent) <= DATALEN)
			totalPacketLength = fileSize+1-totalSizeSent;
		else 
			totalPacketLength = DATALEN;
		memcpy(sends, (buf+totalSizeSent), totalPacketLength);
	
		//Sending packet
		n = sendto(sockfd, &sends, totalPacketLength, 0,serverAddress,socketLength);
		if(n == -1) 
		{
			printf("Error in sending packets, packetSize= %d",strlen(sends));								
			exit(EXIT_FAILURE);
		}
		//Checking for Acknowledgement
		if (recv(sockfd, &acknowledgement, 2, 0) == -1)                                   //receive the ack
		{
		printf("error receiving acknowledgement\n");
		exit(EXIT_FAILURE);
		}
		else{
		if (acknowledgement.num != 1|| acknowledgement.len != 0)
		printf("error in transmission\n");
		else
		printf("Acknowledgement received\n");	
		}
		totalSizeSent += totalPacketLength;
	}
	gettimeofday(&timeOfReceiving, NULL);
	
	*len= totalSizeSent;
	//Getting time interval between client sending and server receiving
	getTimeInterval(&timeOfReceiving, &timeOfSending);
	timeInterval += (timeOfReceiving.tv_sec)*1000.0 + (timeOfReceiving.tv_usec)/1000.0;
	return(timeInterval);
}
void getTimeInterval(struct  timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out ->tv_sec;
		out ->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
