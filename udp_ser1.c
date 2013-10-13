/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/
#include "headsock.h"

// transmitting and receiving function
void transmitAndReceivePackets(int sockfd);                                                           

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in myAddress;
	
	//create socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			
		printf("error in socket");
		exit(EXIT_FAILURE);
	}

	//Initialize Socket Address
	myAddress.sin_family = AF_INET;
	myAddress.sin_port = htons(MYUDP_PORT);
	myAddress.sin_addr.s_addr = INADDR_ANY;
	bzero(&(myAddress.sin_zero), 8);

	//Bind Socket
	if (bind(sockfd, (struct sockaddr *) &myAddress, sizeof(struct sockaddr)) == -1) {           
		printf("error in binding");
		exit(EXIT_FAILURE);
	}
	printf("start receiving\n");
	while(1) {
		transmitAndReceivePackets(sockfd);                        
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
}

void transmitAndReceivePackets(int sockfd)
{
	char receivedMessage[MAXSIZE];
	int n = 0, clientAddresslength = 0;
	struct sockaddr_in clientAddress;
	int datagramCount = 0;	
	clientAddresslength = sizeof (struct sockaddr_in);
	//Initializing acknowledgement
	struct ack_so acknowledgement;
	acknowledgement.num = 1;
	acknowledgement.len = 0;
	
	
	//receive the packet
	if ((n=recvfrom(sockfd, &receivedMessage, DATALEN, 0, (struct sockaddr *)&clientAddress, &clientAddresslength)) > 0) {      
		datagramCount++;
		printf("Received Datagram %d \n",datagramCount);
		printf("the received Datagram is :%s\n", receivedMessage);	

		//Send acknowledgement on receiving the datagram (Stop & Wait Protocol)
		if ((sendto(sockfd,&acknowledgement,2,0,(struct sockaddr *)&clientAddress, clientAddresslength)) < 0)
		{	
			printf("Error in sending acknowledgement, n = %d\n",n);								
			exit(EXIT_FAILURE);
		}
		else
			printf("Acknowledgement sent\n");
	}
	else{
		printf("Error receiving Datagram,n=%d\n",n);
		exit(EXIT_FAILURE);
	}
	
	receivedMessage[n] = '\0';
	printf("the received string is :%s\n", receivedMessage);
}
