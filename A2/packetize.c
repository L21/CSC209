#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "data.h"


FILE           *logfp = NULL;


/* Read a file, break it into packets. */

/*
 * Notes: getopt is a useful library function to make it easier to read in
 * command line arguments, especially those with options. Read the man page
 * (man 3 getopt) for more information.
 */

int main(int argc, char *argv[]) {
    FILE *infp = stdin;
    FILE *outfp = NULL;
    char opt;
    extern int optind;
    extern char *optarg; 
    int i = 0;
    int n;

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                infp = fopen(optarg, "r");
                if(!infp) {
                    perror("fopen");
                    exit(1);
                }
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-f inputfile ] outputfile\n",
                        argv[0]);
                exit(1);
            }
    }

    if(optind >= argc) {
        fprintf(stderr, "Expected outputfile name\n");
        exit(1);
    }

    if(!(outfp = fopen(argv[optind], "w"))){
        perror("fopen");
        exit(1);
    }

    /* The files have been opened for you.  Write the rest of the program here.*/
    
    unsigned char *string = malloc(MAXSIZE*sizeof(char));    
    while (!feof(infp)) {
          n = fread(string, 1, MAXSIZE, infp);
          unsigned short crc;
          crc = crc_message(XMODEM_KEY,string,n);
          struct packet *p = creat_packet(i, n, crc, string);
          i +=1;
          fwrite(p, sizeof(p), 1,outfp);
          fwrite(p->payload, 1,MAXSIZE,outfp);
    }
    fclose(infp);
    fclose(outfp);

    return 0;
}
struct packet *creat_packet(unsigned short block_num,unsigned short block_size,unsigned short crc, unsigned char *payload){
	struct packet *p = malloc(sizeof(struct packet));
	p->block_num = block_num;
	p->block_size = block_size;
	p->crc = crc;
	p->payload = payload;
	return p;
}


	
	
	
	
