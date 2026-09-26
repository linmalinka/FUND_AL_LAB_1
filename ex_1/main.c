#include <stdio.h>
#include "flags.h"


int main(int argc, char *argv[])
{
    long long x;
    char flag;
    Handler handler;
    Status status;

    const Command commands[] = {
        {'h', handle_h},
        {'p', handle_p},
        {'s', handle_s},
        {'e', handle_e},
        {'a', handle_a},
        {'f', handle_f}
    };

    const size_t command_count =
        sizeof(commands) / sizeof(commands[0]);

    if (argc != 3) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    status = parse_ll(argv[1], &x);

    if (status == STATUS_OK) {
        status = parse_flag(argv[2], &flag);
    }

    if (status == STATUS_OK) {
        status = find_handler(
            flag,
            commands,
            command_count,
            &handler
        );
    }

    if (status == STATUS_OK) {
        status = handler(x);
    }
    
    switch (status) {
    case STATUS_OK:
        return 0;

    case STATUS_INVALID_ARGUMENT:
        printf("Invalid argument\n");
        break;

    case STATUS_INVALID_NUMBER:
        printf("Invalid number\n");
        break;

    case STATUS_INVALID_FLAG:
        printf("Invalid flag\n");
        break;

    case STATUS_OUT_OF_RANGE:
        printf("Value is out of allowed range\n");
        break;

    case STATUS_OVERFLOW:
        printf("Overflow\n");
        break;

    case STATUS_OUTPUT_ERROR:
        printf("Output error\n");
        break;
    
    default:
        printf("Unknown error\n");
        return 1;
    }
    return 1;
    
}