#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "data.h"

FILE           *logfp = NULL;


/* Read a packet file, and coalesce packets */

/*
 * Print to stdout the reconstructed data.  If a packet is missing, insert
 * ### into the output
 */

int main(int argc, char *argv[]) {
    FILE *infp;
    char opt;
    extern int optind;
    extern char *optarg;

    while ((opt = getopt(argc, argv, "l:")) != -1) {
        switch (opt) {
            case 'l':
                logfp = fopen(optarg, "w");
                if(!logfp) {
                    perror("fopen");
                    exit(1);
                }
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-l logfile ] inputfile\n",
                        argv[0]);
                exit(1);
        }
    }
    if(optind >= argc) {
        fprintf(stderr, "Expected inputfile name\n");
        exit(1);
    }

    if(argc != 2 && argc != 4) {
        fprintf(stderr, "Usage: %s [-l logfile ] inputfile\n", argv[0]);
        exit(1);    
    }

    if((infp = fopen(argv[optind], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }
    return 0;
}

struct list *creat_node(FILE *infp){
	int n;
	struct list *node = malloc(sizeof(List));
	struct packet *packet_1;
	n = fread(packet_1,sizeof(packet_1),1,infp);
	node->p = packet_1;
	node->next = malloc(sizeof(list));
}	

void log_message(struct packet *p, int message_no, FILE * logfp){
	
