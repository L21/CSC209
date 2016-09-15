#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>


int main(int argc, char *argv[]) {
	extern int optind;
	if(optind >= argc) {
	        fprintf(stderr, "Expected input number\n");
	        exit(1);
	}
	int input;
	input = atoi(argv[1]);

    double inputnumber = (double)input;
	int pid;
	int status;
	int q = 2;
	
	
	//creat an array to save the factors
	int result[2];
	result[1] = 0;
	result[0] = 0;
	int result_index = 0;
	
	
	// get the square root of the input number, if it is not an integer take it's lower bound
	double sqrt_of_number = sqrt(inputnumber);
	int sqrt_number = 0;
	while (sqrt_number <=  sqrt_of_number) {
		sqrt_number = sqrt_number + 1;
	}
	sqrt_number = sqrt_number - 1;
	
	
	
    int prime_number[input];
    int x;
    for (x = 0; x <input; x++) {
	   prime_number[x] = q;
	   q = q+1;
    }
    int index_mark = 0;
    while (index_mark < input) {
	  while(prime_number[index_mark] == 1){
		index_mark = index_mark +1;
	}
	  int index_mark_sub;
	  for (index_mark_sub = index_mark+1;index_mark_sub < input; index_mark_sub++ ) {
		if (prime_number[index_mark_sub] == 1) {
			continue;
		}else if (prime_number[index_mark_sub] % prime_number[index_mark] == 0) {
			prime_number[index_mark_sub] = 1;
		}else if (prime_number[index_mark_sub] % prime_number[index_mark] != 0) {
			continue;
		}
	  }
	  index_mark = index_mark +1;	
    }


    // start send the number, and creat the pipe
    int fd[2];
	if (pipe(fd)== -1) {
	    perror( "pipe failed\n");
		exit(1);
	}
	int first_prime_stage;
	int stage = 0;
	for (q = 2; q <= input; q++ ) {
		
		 // if before it's square root it doesnt have a factor, then it is prime.
		if(q > sqrt_of_number && result[0] == 0){
			break;
		}else if(q > sqrt_of_number && result[0] != 0 && result[1] == 0){
			int another_factor;
			for(another_factor = q ; another_factor <= input;another_factor++){
				if (another_factor *result[0] == input){
					result[1] = another_factor;
					break;
				}
			}
			stage = first_prime_stage;
			break;
		}
		int stage_number = 0;
		// when there is only one factor in result , continue loop.
		if (result[1] == 0) {
			int index = 0;
			while (index < input) {
			   int m ;
			   //get the next prime number in order to know wheher the sent number is prime or not
			   while( prime_number[index] == 1){
				index = index +1;
			   }
			   m = prime_number[index];
			   index= index +1;
			   //creat the process
			   pid = fork();
               //child
			   if(pid == 0){
				   dup2(fd[0],fileno(stdin));
				   if(signal(SIGPIPE, SIG_IGN) ==SIG_ERR) {
	                  perror("signal");
	                  exit(1);
	               }
				   if(execl("./checkprime","checkprime",(char*)0) ==-1){
					   printf("execl wrong\n");
				   }
				  
			   }
			   //parent
		       if(pid >0){
                  //write the data to the pipe.
				  int *input_number = &input;
				  int *input_m = &m;
				  int *input_q = &q;
				  if(write(fd[1], input_number,sizeof(int)) == -1){
					perror( "write input number failed\n");
				    exit(1);
				  }
				  if(write(fd[1], input_m,sizeof(int)) == -1){
					perror("write stage failed\n");
					exit(1);
				  }
				  if(write(fd[1], input_q,sizeof(int)) == -1){
					perror( "write send number failed\n");
					exit(1);
				  }
				  
	              if(wait(&status) == -1){
		            perror( "wait failed\n");
					exit(1);
	              }
			      if(WEXITSTATUS(status) == 3){

				     result[result_index] = q;
				     if(q*q == input){
					    result[result_index+1] = q;
				     }
				     result_index = result_index + 1;
				     stage_number = stage_number + 1;
				     first_prime_stage = stage_number;
				     break;
			      }else if(WEXITSTATUS(status) == 2){
				     stage_number = stage_number + 1;
			         break;
				  }else if(WEXITSTATUS(status) == 6){
					 stage_number = stage_number + 1;
	                 break;
	              }else if (WEXITSTATUS(status) == 4) {
		             stage_number = stage_number + 1;
		             continue;
	              }
               }
			}
		stage = stage_number;
		}else if (result[1] != 0) {
			break;
		}		
	}
	
	//close the pipe
	close(fd[0]);
	close(fd[1]);
	
	
	if(input == 1){
		printf("%d is not the product of two primes\n", input);
		fprintf(stderr, "Number of stages = %d\n", stage);
	}else if(result[0] == 0){
		printf("%d is prime\n", input);
		fprintf(stderr, "Number of stages = %d\n", stage);
	}else if (result[1] == result[0] ) {
		printf("%d %d %d\n", input, result[0], result[0]);
		fprintf(stderr, "Number of stages = %d\n", stage);
	}else if (result[1] * result[0] < input && result[1] != 0 && result[1] != 0) {
		printf("%d is not the product of two primes\n", input);
		fprintf(stderr, "Number of stages = %d\n", stage);
	}else if (result[1] * result[0] == input && result[1] % result[0] == 0 && result[1] != result[0]) {
		printf("%d is not the product of two primes\n", input);
		fprintf(stderr, "Number of stages = %d\n", stage);
	}else if (result[1] != 0 ) {
		printf("%d %d %d\n", input, result[0], result[1]);
		fprintf(stderr, "Number of stages = %d\n", stage);
	}
    
	return 0;
}
				
