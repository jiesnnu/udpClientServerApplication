/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/
#include "headsock.h"

// transmitting and receiving function
void transmitAndReceivePackets(int sockfd);                                                           

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in my_addr;

	//create socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			
		printf("error in socket");
		exit(EXIT_FAILURE);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	//Bind Socket
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {           
		printf("error in binding");
		exit(EXIT_FAILURE);
	}
	printf("start receiving\n");
	while(1) {
		transmitAndReceivePackets(sockfd);                        // send and receive
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
}

void transmitAndReceivePackets(int sockfd)
{
	char receivedMessage[MAXSIZE];
	int n = 0, len;
	struct sockaddr_in addr;
	int datagramCount = 0;	
	len = sizeof (struct sockaddr_in);
	//Initializing acknowledgement
	struct ack_so acknowledgement;
	acknowledgement.num = 1;
	acknowledgement.len = 0;

	//receive the packet
	if ((n=recvfrom(sockfd, &receivedMessage, DATALEN, 0, (struct sockaddr *)&addr, &len)) == 0) {      
		datagramCount++;
		printf("Received Datagram %d \n",datagramCount);
		//Send acknowledgement on receiving the datagram (Stop & Wait Protocol)
		if ((n = send(sockfd, &acknowledgement, 2, 0))==-1)
		{
			printf("Error in sending acknowledgement");								
			exit(EXIT_FAILURE);
		}
	}
	else{
		printf("Error receiving Datagram\n");
		exit(EXIT_FAILURE);
	}
	receivedMessage[n] = '\0';
	printf("the received string is :\n%s", receivedMessage);
}
