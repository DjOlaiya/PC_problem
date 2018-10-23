#ifndef BBUFF_H
#define BBUFF_H
#include <stdio.h>     /* printf(), fprintf */
#include <stdbool.h>
#include <stdlib.h>    /* [s]rand() */
#include <pthread.h>   /* pthread_... */
#include <semaphore.h> /* sem_... */
#include "bbuff.h"

#define BUFFER_SIZE 10



// creating my buffer as a struct.
typedef struct 
{
    void* buffer[BUFFER_SIZE]; //void buffer
    int next_in, next_out;    //tracker variables
    sem_t available,occupied;  //semaphores to count # of avail and empty spots
    pthread_mutex_t buff_mutex;  //locking mechanism for access to buffer
} buffer_t;


buffer_t candyQ;//do i need to make it static/external?
void bbuff_init(void)
{  
     //init a struct with attr of buffer_t
    //init semaphore variables
   
    sem_init (&candyQ.occupied, 0, 0); //# of occupied slots is 0 on init 
    sem_init(&candyQ.available,0,BUFFER_SIZE); //#of free slots is =to buff size
    pthread_mutex_init(&candyQ.buff_mutex,NULL);
    //set to 1 so that my buff index starts from 1 not 0.
    candyQ.next_in = candyQ.next_out = 1;  
    
    /*create size of individual slots in array
    *here i use a 4 loop as opposed to malloc(sizeof(void*)*BUFFER_SIZE).
    *Cos I wasnt sure what error I kept getting...
    */
    for(int i=0;i<BUFFER_SIZE;i++)
    {
        candyQ.buffer[i]=malloc(sizeof(void*));
    }
}
int candyCount=0;
//update candycount fn
int updatecount(void)
{
    int candyCount=(BUFFER_SIZE - candyQ.next_in +candyQ.next_out); //just testing
    return candyCount;
}


// /* TODO: Check and wait if the buffer is full. Ensure exclusive
//      * access to the buffer and use the existing code to remove an item.
//      * this only needs to take in candy, recall that candy struct has factID+timestamp
//      */
void candyQ_insert(void* item)
{   

    sem_wait(&candyQ.available);//any available spots? if yes then lock
	pthread_mutex_lock(&candyQ.buff_mutex); 	
    candyQ.buffer[candyQ.next_in % BUFFER_SIZE ] = item; //circular buffer implementaion
    candyQ.next_in = (candyQ.next_in + 1) % BUFFER_SIZE; //update next in
    pthread_mutex_unlock(&candyQ.buff_mutex);
	sem_post(&candyQ.occupied); //increment occupied spots
}

//check that buff is non empty
_Bool Q_isempty(void)
{
    if(candyQ.next_in==candyQ.next_out)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void* candyQ_extract(void)
{
    //if(Q_isempty())
    //{
    //    fprintf(stderr,"Sorry, Array is empty");
    //}
    sem_wait(&candyQ.occupied);//any available spots? if yes then lock
	pthread_mutex_lock(&candyQ.buff_mutex); 
    void * candyItem=candyQ.buffer[candyQ.next_out % BUFFER_SIZE];
    candyQ.next_out = (candyQ.next_out + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&candyQ.buff_mutex);
	sem_post(&candyQ.available); //increment o	
    return candyItem;    
}


#endif

