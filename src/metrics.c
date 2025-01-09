#include "metrics.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int total_requests = 0;
static double total_response_time = 0.0;
static int method_counts[4] = {0}; 
static struct route_metric {
    char route[256];
    int count;
} *route_metrics = NULL;
static int route_count = 0;

void increment_request_count() {
    total_requests++;
}

void increment_method_count(const char *method) {
    if (strcmp(method, "GET") == 0) {
        method_counts[0]++;
    } else if (strcmp(method, "POST") == 0) {
        method_counts[1]++;
    } else if (strcmp(method, "PUT") == 0) {
        method_counts[2]++;
    } else if (strcmp(method, "DELETE") == 0) {
        method_counts[3]++;
    }
}

void increment_route_count(const char *route) {
    for (int i = 0; i < route_count; i++) {
        if (strcmp(route_metrics[i].route, route) == 0) {
            route_metrics[i].count++;
            return;
        }
    }

    route_metrics = realloc(route_metrics, sizeof(struct route_metric) * (route_count + 1));
    strcpy(route_metrics[route_count].route, route);
    route_metrics[route_count].count = 1;
    route_count++;
}

clock_t start_request() {
    return clock();
}

void end_request(clock_t start_time) {
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    total_response_time += elapsed_time;
}

void display_metrics() {
    printf("\n=== Server Metrics ===\n");
    printf("Total Requests: %d\n", total_requests);
    printf("Average Response Time: %.3f seconds\n", total_response_time / (total_requests > 0 ? total_requests : 1));
    printf("Requests by Method:\n");
    printf("  GET: %d\n", method_counts[0]);
    printf("  POST: %d\n", method_counts[1]);
    printf("  PUT: %d\n", method_counts[2]);
    printf("  DELETE: %d\n", method_counts[3]);
    printf("Requests by Route:\n");
    for (int i = 0; i < route_count; i++) {
        printf("  %s: %d\n", route_metrics[i].route, route_metrics[i].count);
    }
    printf("======================\n");
}
