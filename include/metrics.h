#ifndef METRICS_H
#define METRICS_H

#include <time.h>

void increment_request_count();

void increment_method_count(const char *method);

clock_t start_request();

void end_request(clock_t start_time);

void display_metrics();

void save_general_metrics();

void save_requests_by_method();

void save_metrics_to_csv();

#endif
