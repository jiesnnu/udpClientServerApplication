/*******************************
udp_client.c: the source file of the client in udp
********************************/

#include "headsock.h"

float transmit_packets(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, long *len);                
void get_time_interval(struct  timeval *out, struct timeval *in);

int main(int argc, char *argv[])
{
	long len;
	int sockfd;
	struct sockaddr_in ser_addr;
	char **pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	float rate_of_transmission;
	float time_interval;
	FILE* file_pointer;

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
	if((file_pointer = fopen ("myfile.txt","r+t")) == NULL)
	{
		printf("File doesn't exist\n");
		exit(EXIT_FAILURE);
	}
	time_interval = transmit_packets(file_pointer, sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in), &len);   
	rate_of_transmission = (len/(float)time_interval);
	printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", time_interval, (int)len, rate_of_transmission);
	close(sockfd);
	exit(EXIT_SUCCESS);
}

float transmit_packets(FILE *fp, int sockfd, struct sockaddr *server_address, int addrlen, long *len)
{
	char *buf;
	long file_size, total_size_sent;
	char sends[DATALEN];
	int socket_length = sizeof(struct sockaddr_in);
	struct ack_so acknowledgement;
	//Packet length + Header Length = total_packet_length
	int n, total_packet_length;
	float time_interval = 0.0;
	double random_error_probability = 0.0;
	struct timeval time_of_sending, time_of_receiving;
	total_size_sent = 0;

	
	fseek (fp , 0 , SEEK_END);
	file_size = ftell (fp);
	rewind (fp);
	printf("The file length is %d bytes\n", (int)file_size);
	printf("the packet length is %d bytes\n",DATALEN);

	// allocate memory to contain the whole file.
	buf = (char *) malloc (file_size);
	if (buf == NULL) 
		exit (EXIT_FAILURE);

  	// copy the file into the buffer.
	fread (buf,1,file_size,fp);

  	/*** the whole file is loaded in the buffer. ***/
	buf[file_size] ='\0';									
	gettimeofday(&time_of_sending, NULL);
	int retransmit_flag = 0;
	srand(time(NULL));
	while(total_size_sent<= file_size)
	{
		if(retransmit_flag == 1){
			retransmit_flag = 0;
			printf("Retransmitting frame\n");
		}

		if ((file_size+1-total_size_sent) <= DATALEN)
			total_packet_length = file_size+1-total_size_sent;
		else 
			total_packet_length = DATALEN;
		memcpy(sends, (buf+total_size_sent), total_packet_length);
		//Generating a random error probability value
		random_error_probability = (double)rand() / (double)RAND_MAX ;

		if(random_error_probability > FRAME_ERROR_PROBABILITY)
		{
		//Sending packet without any error
		printf("Error Probability less than FRAME_ERROR_PROBABILITY\n",random_error_probability);
		n = sendto(sockfd, &sends, total_packet_length, 0, server_address, socket_length);
		}
		else{
			printf("random_error_probability = %f\nSending bad frame\n",random_error_probability);
			n = sendto(sockfd, &sends, 0, 0, server_address, socket_length);
		}
		if(n == -1) 
			exit(EXIT_FAILURE);
		//Checking for Acknowledgement
		if (recv(sockfd, &acknowledgement, 2, 0) == -1)                                   //receive the ack
		{
		printf("error receiving acknowledgement\n");
		exit(EXIT_FAILURE);
		}
		else{
		if (acknowledgement.num != ACK_CODE || acknowledgement.len != 0)
		{
			printf("Wrong Acknowledgement Received, continuing\n");
			retransmit_flag = 1;
			continue;
			//printf("error in transmission\n");
		}
		else
		printf("Acknowledgement received\n");	
		}
		total_size_sent += total_packet_length;
	}
	gettimeofday(&time_of_receiving, NULL);
	
	*len= total_size_sent;
	//Getting time interval between client sending and server receiving
	get_time_interval(&time_of_receiving, &time_of_sending);
	time_interval += (time_of_receiving.tv_sec)*1000.0 + (time_of_receiving.tv_usec)/1000.0;
	return(time_interval);
}
void get_time_interval(struct  timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out ->tv_sec;
		out ->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
