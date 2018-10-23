#ifndef STATS_H
#define STATS_H

void stats_init(int num_producers);
void stats_cleanup(void);
void stats_produced(int factory_ID);
void stats_eaten(int factory_ID, double delay_in_ms);
void stats_display(void);

#endif