#ifndef METRICS_H
#define METRICS_H

#include <time.h>

void increment_request_count();

void increment_method_count(const char *method);

void increment_route_count(const char *route);

clock_t start_request();

void end_request(clock_t start_time);

void display_metrics();

#endif
