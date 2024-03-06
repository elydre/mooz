#include "kalos.h"

kalos_event_t kalos_events[kalos_events_max_len] = {
	(kalos_event_t){
		.is_pressed = 2,
		.key[0] = 0
		}
	};

int kalos_events_len = 0;

void kalos_sleep_ms(long long int time) {
    long long int start = kalos_get_time_ms();
	while (start + time < kalos_get_time_ms());
}