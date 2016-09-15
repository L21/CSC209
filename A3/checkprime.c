#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MAX 256

int main(void) {
	int n;
	int number;
	int m, num_for_sent;
	int input[sizeof(int)], input_m[sizeof(int)] ,input_num_for_sent[sizeof(int)];
	
	
	if((n = read(STDIN_FILENO, input, sizeof(int))) == -1) {
	        perror("read");
	        exit(1);
	    } else if(n == 0) {
	        fprintf(stderr, "Error: could not read from stdin");
	        exit(1);
	}
	number = *input;

	
	
	
	if((n = read(STDIN_FILENO, input_m, sizeof(int))) == -1) {
		    perror("read");
		    exit(1);
		} else if(n == 0) {
		    fprintf(stderr, "Error: could not read from stdin");
		    exit(1);
		}
	m = *input_m;
	
	
	
	if((n = read(STDIN_FILENO, input_num_for_sent,sizeof(int))) == -1) {
			perror("read");
			exit(1);
		} else if(n == 0) {
			fprintf(stderr, "Error: could not read from stdin");
			exit(1);
		}
	num_for_sent = *input_num_for_sent;
	
    if(num_for_sent * m == number){

	    exit(3);//factor > square root of input
    }
    if (num_for_sent == m) {
	    if(number % num_for_sent != 0){
		    exit(2);// new node
	    }else {
		    exit(3);// factor and new node
	    }
	}else{
		if((num_for_sent % m) != 0){
		    exit(4);// not the mult of m
	    }else if ((num_for_sent % m) == 0) {
	        if(number % num_for_sent == 0){
		       exit(3);//discard but it is the factor
	        }else {
		       exit(6);//discard and is not the factor
	        }
	    }
	}
    

	return 0;
}
