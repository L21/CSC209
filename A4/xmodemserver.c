#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include "xmodemserver.h"
#include "crc16.h"
#ifndef PORT
  #define PORT 53938
#endif



struct client *top = NULL;
static int listenfd; 


static void removeclient(int fd);
static void addclient(struct client *p, int fd, int state);
int main(int argc, char **argv){
	int nselect, nread;
	struct client *p ;
	extern void bindandlisten();
	
	char temp;
    printf("port is %d\n", PORT);

	
    bindandlisten();
    
	while (1) {
		fd_set fdlist;
		int maxfd = listenfd;
		FD_ZERO(&fdlist);
		FD_SET(listenfd, &fdlist);
		for (p = top; p; p = p->next) {
			FD_SET(p->fd, &fdlist);
			if (p->fd > maxfd){
				maxfd = p->fd;
			}
		}
		
		if((nselect = select(maxfd + 1, &fdlist, NULL, NULL, NULL)) == -1){
			perror("select");
		}else{
			for (p = top; p; p = p->next){
				if (FD_ISSET(p->fd, &fdlist)){
					break;
				}
			}
			
			if (FD_ISSET(listenfd, &fdlist)){
				int fd;
				struct sockaddr_in r;
				socklen_t socklen = sizeof r;
				if ((fd = accept(listenfd, (struct sockaddr *)&r, &socklen)) < 0) {
					perror("accept");
				} 
				p = malloc(sizeof(struct client));
				addclient(p,fd, initial);
//most of the code above is copied from the sample code.
				// creat some unsigned char to store the value
				char *dir = "filestore";
				unsigned char char_block;
				unsigned char char_block_inverse;
				unsigned char payload[128];
				unsigned char payload2[1024];
				unsigned char buf_one_byte;
				unsigned char first_byte;
				unsigned char second_byte;
				unsigned short crc;
				unsigned char crc_byte_high;
				unsigned char crc_byte_low;
				
				
				//while state is not finished , it will nerver finished.
				while (p->state != finished) { 
				
			
					if (p->state == initial) {
					   fprintf(stderr, "initial state\n");
						FD_ZERO(&fdlist);
						FD_SET(fd, &fdlist);
						
						if((nselect = select(fd + 1, &fdlist, NULL, NULL, NULL)) == -1){
							perror("select");
						}
						if (FD_ISSET(fd, &fdlist)){
						
							if ((nread = read(fd, &temp, sizeof(char))) == -1) {
								 perror("read");
							}
							//read the filename
							int index = 0;
							while (temp != '\n') {
								p->filename[index] = temp;
								if ((nread = read(fd, &temp, 1)) == -1) {
									perror("read");
								}
								index++;
							}
						 //open the file , if not exits, creat it
							if((p->fp = open_file_in_dir(p->filename, dir)) == NULL) {
								perror("open file");
								exit(1);
							}
							p->current_block = 1;
							temp = 'C';
							
							if(write(fd,&temp,1) < 0) {
							   fprintf(stderr, "Error: could not write all the data\n");
							}
							
							p->state = pre_block;
						}
					}
				
					if (p->state == pre_block) {
						fprintf(stderr, "move to pre_block\n");
					
						if ((nread = read(fd, &temp, 1)) == -1) {
							perror("read");
						}
						//check the signal to determined what to do next
						if (temp == EOT) {
							temp = ACK;
							if(write(fd, &temp,1) < 0) {
								fprintf(stderr, "Error: could not write all the data\n");
							}
							p->state = finished;
							continue;
						}
						if (temp == SOH){
							p->state = get_block;
						}
						if (temp == STX){
							p->state = get_block;
						}
						
					}
				
					if (p->state == get_block) {
						fprintf(stderr, "move to get_block\n");
						
						if((nselect = select(fd + 1, &fdlist, NULL, NULL, NULL)) == -1){
							perror("select");
						}
						if (FD_ISSET(fd, &fdlist)){
							//if next is 132 byte, since cannot assume in a singal call, just do it one by one
							if (temp == SOH){
								p->blocksize = 132;
								for (p->inbuf = 0; p->inbuf < 132 ; p->inbuf ++) {
									if ((nread = read(fd, &buf_one_byte, 1)) == -1) {
										perror("read");
									}
									p->buf[p->inbuf] = buf_one_byte;
								}
								p->buf[132] = '\0';
								p->state = check_block;
							}
							//if next is 1028 byte, since cannot assume in a singal call, just do it one by one
							if (temp == STX){
								p->blocksize = 1028;
								
								for (p->inbuf = 0; p->inbuf < 1028 ; p->inbuf ++) {
									if ((nread = read(fd, &buf_one_byte, 1)) == -1) {
										perror("read");
									}
									p->buf[p->inbuf] = buf_one_byte;
								}
								p->buf[1028] = '\0';
								p->state = check_block;
							}
						}

					}
					
					if (p->state == check_block) {
						fprintf(stderr, "move to check_block\n");
						if (temp == SOH){
							char_block = p->buf[0];
							char_block_inverse = p->buf[1];
							int crc_index;
							for (crc_index = 0; crc_index < 128; crc_index ++) {
								payload[crc_index] = p->buf[crc_index +2];
							}
							crc = crc_message(XMODEM_KEY, payload, 128);
							first_byte = p->buf[130];
							second_byte = p->buf[131];
						}
						if (temp == STX){
							char_block = p->buf[0];
							char_block_inverse = p->buf[1];
							int crc_index;
							for (crc_index = 0; crc_index < 1024; crc_index ++) {
								payload2[crc_index] = p->buf[crc_index +2];
							}
							crc = crc_message(XMODEM_KEY, payload2, 1024);
							first_byte = p->buf[1026];
							second_byte = p->buf[1027];
						}
						// check the block number and inverse number
						if (char_block + char_block_inverse != 255) {
							p->state = finished;
							continue;
						}
						// check the block number 
						if (char_block == p->current_block -1) {
							temp = ACK;
							if(write(fd, &temp,1) < 0) {
								fprintf(stderr, "Error: could not write all the data\n");
							}
						}
						
						if (char_block != p->current_block) {
							p->state = finished;
							continue;
						}
						crc_byte_high = crc >> 8;
						crc_byte_low = crc ;
						//check the crc
						if ((second_byte != crc_byte_low) || (first_byte != crc_byte_high) ) {
							temp = NAK;
							if(write(fd, &temp,1) < 0) {
							   fprintf(stderr, "Error: could not write all the data\n");
							}
							p->state = pre_block;
							continue;
						}
						//write to the file
						temp = ACK;
						if(write(fd, &temp,1) < 0) {
							fprintf(stderr, "Error: could not write all the data\n");
						}
						if(fwrite(p, sizeof(struct client), 1, p->fp) !=1 ){
							perror("fail to write");
						}
						p->state = pre_block;
						p->current_block ++;
						
					}
				}
				//clean all the files
				if (p->state == finished) {
					fprintf(stderr, "Finished\n");
					close(fd);
					if(fclose(p->fp) == EOF){
						perror("fclose failed");
					}
					removeclient(fd);
						
				}
			
			}
		}	
	}
	
	
		
}

//the following 3 method are copy from the sample code, and fix a little bit.

void bindandlisten()  /* bind and listen, abort on error */
{
    struct sockaddr_in r;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("socket");
	exit(1);
    }

    memset(&r, '\0', sizeof r);
    r.sin_family = AF_INET;
    r.sin_addr.s_addr = INADDR_ANY;
    r.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&r, sizeof r)) {
	perror("bind");
	exit(1);
    }

    int yes = 1;
    if((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
        perror("setsockopt");
    }

    if (listen(listenfd, 10)) {
	perror("listen");
	exit(1);
    }
}

static void addclient(struct client *p,int fd, int state)
{
//    struct client *p = malloc(sizeof(struct client));
    if (!p) {
	fprintf(stderr, "out of memory!\n");  /* highly unlikely to happen */
	exit(1);
    }
    printf("Adding client\n");
    fflush(stdout);
  
    p->fd = fd;
    p->next = top;
    
    p->state = state;
    
    top = p;
   
}



static void removeclient(int fd)
{
    struct client **p;
    for (p = &top; *p && (*p)->fd != fd; p = &(*p)->next)
	;
    if (*p) {
	struct client *t = (*p)->next;
	printf("Removing client\n");
	fflush(stdout);
	free(*p);
	*p = t;
    } else {
	fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n", fd);
	fflush(stderr);
    }
}

