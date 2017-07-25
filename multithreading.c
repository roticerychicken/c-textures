#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//sum computed by the background thread
struct sum_runner_struct {
	long long limit;
	long long answer;
};

// Thread function to generate sum of 0 to N
void* sum_runner(void* arg)
{
	struct sum_runner_struct *arg_struct = (struct sum_runner_struct*) arg;

	long long sum = 0;
	for(long long i = 0; i <= arg_struct->limit; i++) {
		sum += i; 
	}
	//sum is a global variable so other threads can access

	arg_struct->answer = sum;

	pthread_exit(0);
	
}
int main(int argc, char **argv)
{
	
	int num_args = argc - 1;

        //Thread ID:
        pthread_t tids[num_args];
	
        //create attributes
        struct sum_runner_struct args[num_args]; 

	for(int i = 0; i < num_args; i++) {
		
		args[i].limit = atoll(argv[i+1]);

		pthread_attr_t attr;
        	pthread_attr_init(&attr);
        	pthread_create(&tids[i],&attr,sum_runner,&args[i]);
    	}

	//wait until thread is done
	for(int i = 0; i < num_args; i++) {
		pthread_join(tids[i],NULL);
		printf("Sum for thread %d is %lld\n",i,args[i].answer);
	}
}


