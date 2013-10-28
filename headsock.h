// headfile for UDP program
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#define NEWFILE (O_WRONLY|O_CREAT|O_TRUNC)
#define MYTCP_PORT 4950
#define MYUDP_PORT 5350
#define DATALEN 300
#define BUFSIZE 60000
#define PACKLEN 308
#define HEADLEN 8
#define FRAME_ERROR_PROBABILITY 0.20
#define NACK_CODE -1
#define ACK_CODE   1
#define TIMEOUT_INTERVAL 1000000 //timeout interval in microseconds for usleep()

struct pack_so					//data packet structure
{
uint32_t num;					// the sequence number
uint32_t len;					// the packet length
char data[DATALEN];				//the packet data
};	

struct ack_so
{
uint8_t num;
uint8_t len;
};
