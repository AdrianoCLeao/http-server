#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <unistd.h>
#endif

static int total_requests = 0;
static double total_response_time = 0.0;
static int method_counts[4] = {0}; 

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

clock_t start_request() {
    return clock();
}

void end_request(clock_t start_time) {
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    total_response_time += elapsed_time;
}

size_t get_memory_usage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize; 
    }
    return 0;
#else
    FILE *file = fopen("/proc/self/statm", "r");
    if (!file) return 0;

    long pages;
    if (fscanf(file, "%ld", &pages) != 1) {
        fclose(file);
        return 0;
    }
    fclose(file);

    return pages * sysconf(_SC_PAGESIZE); 
#endif
}

void display_metrics() {
    printf("\n=== Server Metrics ===\n");
    printf("Total Requests: %d\n", total_requests);
    printf("Average Response Time: %.3f ms\n", 1000*(total_response_time / (total_requests > 0 ? total_requests : 1)));
    printf("Requests by Method:\n");
    printf("  GET: %d\n", method_counts[0]);
    printf("  POST: %d\n", method_counts[1]);
    printf("  PUT: %d\n", method_counts[2]);
    printf("  DELETE: %d\n", method_counts[3]);
    printf("Memory Usage: %.2f MB\n", get_memory_usage() / (1024.0 * 1024.0));
    printf("======================\n");
}

void save_general_metrics() {
    FILE *file;
    char metrics_dir[] = "metrics";

    struct stat st;
    if (stat(metrics_dir, &st) != 0) {
#ifdef _WIN32
        mkdir(metrics_dir);
#else
        mkdir(metrics_dir, 0755);
#endif
    }

    file = fopen("metrics/general_metrics.csv", "w");
    if (!file) {
        perror("Failed to create general_metrics.csv");
        return;
    }

    fprintf(file, "Metric,Value\n");
    fprintf(file, "Total Requests,%d\n", total_requests);
    fprintf(file, "Average Response Time,%.3f\n", 1000*(total_response_time / (total_requests > 0 ? total_requests : 1)));
    fprintf(file, "Memory Usage,%.2f\n", get_memory_usage() / (1024.0 * 1024.0));
    fclose(file);
}

void save_requests_by_method() {
    FILE *file;

    file = fopen("metrics/requests_by_method.csv", "w");
    if (!file) {
        perror("Failed to create requests_by_method.csv");
        return;
    }

    fprintf(file, "Method,Requests\n");
    fprintf(file, "GET,%d\n", method_counts[0]);
    fprintf(file, "POST,%d\n", method_counts[1]);
    fprintf(file, "PUT,%d\n", method_counts[2]);
    fprintf(file, "DELETE,%d\n", method_counts[3]);
    fclose(file);
}

void save_metrics_to_csv() {
    save_general_metrics();
    save_requests_by_method();
}
