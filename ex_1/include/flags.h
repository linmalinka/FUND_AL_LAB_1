#ifndef FLAGS_H
#define FLAGS_H

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

typedef Status (*Handler)(long long x);

typedef struct {
    char flag;
    Handler handler;
} Command;

Status parse_ll(const char *str, long long *value);
Status parse_flag(const char *str, char *flag);

Status find_handler(
    char flag,
    const Command *commands,
    size_t count,
    Handler *handler
);

Status handle_h(long long x);
Status handle_p(long long x);
Status handle_s(long long x);
Status handle_e(long long x);
Status handle_a(long long x);
Status handle_f(long long x);

#endif