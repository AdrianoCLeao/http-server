#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h> 
#else
    #include <unistd.h>
#endif

#define LOG_DIR "logs"
#define LOG_FILE "logs/log.csv"

void write_log(const char *method, const char *route, const char *client_ip) {
    FILE *log_file;

    struct stat st;
    if (stat(LOG_DIR, &st) != 0) {
#ifdef _WIN32
        if (_mkdir(LOG_DIR) != 0) {
            perror("Failed to create logs directory");
            return;
        }
#else
        if (mkdir(LOG_DIR, 0755) != 0) {
            perror("Failed to create logs directory");
            return;
        }
#endif
    }

    log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

    fprintf(log_file, "%s,%s,%s,%s\n", timestamp, method, route, client_ip);
    fclose(log_file);
}
