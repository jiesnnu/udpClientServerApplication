/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/
#include "headsock.h"

// transmitting and receiving function
void transmit_and_receive_packets(int sockfd);                                                           

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in my_address;
	
	//create socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			
		printf("error in socket");
		exit(EXIT_FAILURE);
	}

	//Initialize Socket Address
	my_address.sin_family = AF_INET;
	my_address.sin_port = htons(MYUDP_PORT);
	my_address.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_address.sin_zero), 8);

	//Bind Socket
	if (bind(sockfd, (struct sockaddr *) &my_address, sizeof(struct sockaddr)) == -1) {           
		printf("error in binding");
		exit(EXIT_FAILURE);
	}
	printf("start receiving\n");
	while(1) {
		transmit_and_receive_packets(sockfd);                        
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
}

void transmit_and_receive_packets(int sockfd)
{
	char received_message[MAXSIZE];
	int n = 0, client_address_length = 0;
	struct sockaddr_in client_address;
	int datagramCount = 0;	
	client_address_length = sizeof (struct sockaddr_in);
	//Initializing acknowledgement
	struct ack_so acknowledgement;
	acknowledgement.num = 1;
	acknowledgement.len = 0;
	
	
	//receive the packet
	if ((n=recvfrom(sockfd, &received_message, DATALEN, 0, (struct sockaddr *)&client_address, &client_address_length)) > 0) {      
		datagramCount++;
		printf("Received Datagram %d \n",datagramCount);
		printf("the received Datagram is :%s\n", received_message);	

		//Send acknowledgement on receiving the datagram (Stop & Wait Protocol)
		if ((sendto(sockfd,&acknowledgement,2,0,(struct sockaddr *) &client_address, client_address_length)) < 0)
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
	
	received_message[n] = '\0';
	printf("the received string is :%s\n", received_message);
}
