#include <stdio.h>
#include "bbuff.h"
#include "stats.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include<sys/time.h>

_Bool stop_thread=false;
//####################################candy object################################################
typedef struct  
{
    int f_ID;
    double timeStamp_ms;
}   candy_t;

//####################################time stamp################################################
double currTime_ms(void) //give fn to get time in seconds
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now); //what header to get rid of this error?
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}


//####################################Factory Thread################################################
//fn will
void *factory_runner(void *arg1) 
{ 
	int factory_ID = *(int*) arg1;
	while(!stop_thread)
	{ 
	 	int wait_time=rand()% 4; //rand generate a value between 0 and 3
		//instantiate candy since only producer right now
		candy_t *nu_candy =malloc(sizeof(candy_t)); 
		if(!nu_candy)
		{
			fprintf(stderr,"factory is down, sorry brah\n");
		}
		//declare values stored in candy item
	 	nu_candy->f_ID = factory_ID ;
	 	nu_candy->timeStamp_ms = currTime_ms();
		stats_produced(nu_candy->f_ID);
		candyQ_insert(nu_candy);
		printf("Factory %d will now ship candy and wait %d seconds\n",factory_ID,wait_time );
		sleep(wait_time); //sleep for req'd time
	 }
	printf("Factory %d is done\n",factory_ID );
	free(arg1); //change to free nucandy   
	pthread_exit(0);
}

void *fatKid(void* arg)
{
	while(true)
	{
	
		int wait_time=rand()% 4; //rand generate a value between 0 and 3
		candy_t *nu_candy =candyQ_extract();
		double candy_produced_time = nu_candy->timeStamp_ms;
        double current_time = currTime_ms();
        double delay_in_ms = current_time - candy_produced_time;
        stats_eaten(nu_candy->f_ID, delay_in_ms);
        free(nu_candy);
		sleep(wait_time);

	}

	
}



//####################################Main################################################
int main (int argc, char *argv[])
{
	// 1.  Extract arguments
   
	int values[3]; //now can access values from anywhere
  	char *varnames[]= {"Factories","Kids","Factory LifeSpan"}; 
  //base case if arguments don't equal 3 exactly
  	if (argc!=4)
  	{
    	fprintf(stderr, "Wrong number of inputs. Must have 3 arguments\n" );
    	exit(-1);
  	}
  	else
  	{
  		int num_args = argc-1;
  		for (int i=0;i<num_args;i++)
    	{
     		values[i]=atoi(argv[i+1]); //string to int converter
    	}
   		if (values[0]<=0||values[1]<=0||values[2]<=0)
      	{
			fprintf(stderr,"Y u so stupid,you can't have 0 Arguments\n Each argument must be a positive int.TRY AGAIN\n");
      		exit(-1);
      	}		
    	else 
      	{
    		printf("The number of factories is %d\n",values[0]);
        	printf("The number of kids is: %d\n", values[1]);
        	printf("The lifespan of the factories is %d\n", values[2]);
		
		// 2.  Init modules
            bbuff_init();
            stats_init(values[1]);

            // 3.  init factory threads
            pthread_t fact_threads[values[1]];
            for(int i = 0; i< values[1]; i++)
			{
                int *init_var = malloc(sizeof(int)); 
                *init_var= i;
                pthread_create(&fact_threads[i], NULL, factory_runner, init_var);
            }

            // 4.  init kid threads
            pthread_t kids_thread[values[2]];
            for(int i = 0; i< values[2]; i++)
			{
                pthread_create(&kids_thread[i], NULL, fatKid, NULL);  
            }

            // 5.  Lifespan
            for(int i = 0; i<values[3]; i++)
			{
                printf("Time %ds:\n", i);
                sleep(1);
            }
            
            // 6.  Stop factory threads
            stop_thread = true;
            for(int i = 0; i<values[1];i++)
			{
                pthread_join(fact_threads[i], NULL);
            }

            // 7.  Wait until candy done
            while(!Q_isempty())
			{
                printf("Waiting for remaining candy to be consumed.\n");
                sleep(1);
            };

            //8. Stop kid thread
            printf("Stopping kids.\n");
            for(int i = 0; i< values[2]; i++)
			{
                int thread_end = pthread_cancel(kids_thread[i]);
                if(thread_end!=0)
				{
                printf("Error. Could not cancel Pthread.\n");
                pthread_join(kids_thread[i], NULL);
            	}
        	}
		

        // 9.  Print statistics
        stats_display();
		} 
	}
  	
  	
}
