#include "task3.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    char flag;
    Status status;
    const Command *command;

    const Command commands[] = {
        {'q', 4, handle_q},
        {'m', 2, handle_m},
        {'t', 4, handle_t}
    };

    const size_t command_count =
        sizeof(commands) / sizeof(commands[0]);

    if (argc < 2) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    status = parse_flag(argv[1], &flag);

    if (status == STATUS_OK) {
        status = find_command(
            flag,
            commands,
            command_count,
            &command
        );
    }

    if (status == STATUS_OK &&
        (size_t)(argc - 2) != command->argument_count) {
        status = STATUS_INVALID_ARGUMENT;
    }

    if (status == STATUS_OK) {
        status = command->handler(argv + 2);
    }

    switch (status) {
    case STATUS_OK:
        return 0;

    case STATUS_INVALID_ARGUMENT:
        printf("Invalid arguments\n");
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
        return 1;

    default:
        printf("Unknown error\n");
        break;
    }

    return 1;
}