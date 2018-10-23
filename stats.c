#ifndef STATS_H
#define STATS_H
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "bbuff.h"
#define MAX_FACTORY 1000

typedef struct {
    int factory_number;
    int candy_made;
    int candy_eaten;
    float min_delay;
    float max_delay;
    float avg_delay;
    float total_delay;
}factory_t;


factory_t *stats_data[MAX_FACTORY];
int num_of_factories;

void stats_init(int num_producers){
    num_of_factories = num_producers;
    for(int i = 0; i<num_producers; i++){
        stats_data[i] =  malloc(sizeof(factory_t));
        stats_data[i]->factory_number = i;
        stats_data[i]->candy_made = 0;
        stats_data[i]->candy_eaten = 0;
        stats_data[i]->min_delay = 0.0;
        stats_data[i]->max_delay= 0.0;
        stats_data[i]->avg_delay=0.0;
        stats_data[i]->total_delay=0.0; 
    }
    
};
void stats_cleanup(void){
    for(int i = 0; i<MAX_FACTORY; i++){
        free(stats_data[i]);
    }
    free(*stats_data);
}
void stats_produced(int factory_number){
    stats_data[factory_number]->candy_made++;
}

void stats_eaten(int factory_number, double delay_in_ms){
    double candy_delay_time  = delay_in_ms;
    if(stats_data[factory_number]->candy_made ==1){
        stats_data[factory_number]-> min_delay = candy_delay_time;
        stats_data[factory_number]-> max_delay = candy_delay_time;
        stats_data[factory_number]-> total_delay = candy_delay_time;
        stats_data[factory_number]->candy_eaten++;
    }
    else{
        if(candy_delay_time<stats_data[factory_number]->min_delay){
            stats_data[factory_number]->min_delay = candy_delay_time;
        }
        if(candy_delay_time>stats_data[factory_number]->max_delay){
            stats_data[factory_number]->max_delay = candy_delay_time;
        }
        stats_data[factory_number]->candy_eaten++;
        stats_data[factory_number]-> total_delay += candy_delay_time;
    }
}
void stats_display(void){
    printf("Statistics:\n");
    printf("%8s%10s%10s%15s%15s%15s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]" );
    for(int i = 0; i<num_of_factories; i++){
        printf("%8d%10d%10d%15f%15f%15f\n", i, stats_data[i]->candy_made, stats_data[i]->candy_eaten, stats_data[i]->min_delay, 
            (stats_data[i]->total_delay / stats_data[i]->candy_made), stats_data[i]->max_delay);
            if(stats_data[i]->candy_made != stats_data[i]->candy_eaten){
                printf("\tERROR: Mismatch between number made and eaten\n");
            }
    }
    
}

#endif