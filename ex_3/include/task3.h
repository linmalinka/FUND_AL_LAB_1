#ifndef TASK3_H
#define TASK3_H

#include <stddef.h>

typedef enum {
    STATUS_OK = 0,
    STATUS_INVALID_ARGUMENT,
    STATUS_INVALID_NUMBER,
    STATUS_INVALID_FLAG,
    STATUS_OUT_OF_RANGE,
    STATUS_OVERFLOW,
    STATUS_OUTPUT_ERROR
} Status;

typedef Status (*Handler)(char *args[]);

typedef struct {
    char flag;
    size_t argument_count;
    Handler handler;
} Command;

Status parse_ll(const char *str, long long *value);
Status parse_double(const char *str, double *value);
Status parse_flag(const char *str, char *flag);

Status find_command(
    char flag,
    const Command *commands,
    size_t count,
    const Command **command
);

Status handle_q(char *args[]);
Status handle_m(char *args[]);
Status handle_t(char *args[]);

#endif