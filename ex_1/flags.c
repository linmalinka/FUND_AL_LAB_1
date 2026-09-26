#include "flags.h"
#include <stdio.h>
#include <limits.h>

#define MULTIPLES_LIMIT 100
#define HEX_DIGITS_COUNT 16
#define MAX_POWER_BASE 10
#define MAX_POWER_DEGREE 10

typedef enum {
    PRIME_NEITHER,
    PRIME_YES,
    PRIME_COMPOSITE
} PrimeResult;

static Status print_ull(unsigned long long value);

static Status find_multiples(long long x, int values[MULTIPLES_LIMIT], size_t *count);
static Status print_multiples(const int values[MULTIPLES_LIMIT], size_t count);

static Status check_prime(long long x, PrimeResult *result);
static Status print_prime_result(PrimeResult result);

static Status get_hex_digits(long long x, char digits[HEX_DIGITS_COUNT], size_t *count);
static Status print_hex_digits(const char digits[HEX_DIGITS_COUNT], size_t count);

static Status build_power_table(
    long long x,
    unsigned long long table[MAX_POWER_BASE][MAX_POWER_DEGREE]
);

static Status print_power_table(
    unsigned long long table[MAX_POWER_BASE][MAX_POWER_DEGREE],
    long long x
);

static Status calculate_sum(
    long long x,
    unsigned long long *result
);

static Status calculate_factorial(
    long long x,
    unsigned long long *result
);

Status parse_ll(const char *str, long long *value)
{
    unsigned long long result = 0;
    unsigned long long limit;
    size_t i = 0;
    int negative = 0;

    if (str == NULL || value == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (str[0] == '\0') {
        return STATUS_INVALID_NUMBER;
    }

    if (str[i] == '-' || str[i] == '+') {
        negative = (str[i] == '-');
        ++i;

        if (str[i] == '\0') {
            return STATUS_INVALID_NUMBER;
        }
    }

    limit = negative
        ? (unsigned long long)LLONG_MAX + 1ULL
        : (unsigned long long)LLONG_MAX;

    for (; str[i] != '\0'; ++i) {
        unsigned int digit;

        if (str[i] < '0' || str[i] > '9') {
            return STATUS_INVALID_NUMBER;
        }

        digit = (unsigned int)(str[i] - '0');

        if (result > (limit - digit) / 10ULL) {
            return STATUS_OVERFLOW;
        }

        result = result * 10ULL + digit;
    }

    if (negative) {
        if (result == (unsigned long long)LLONG_MAX + 1ULL) {
            *value = LLONG_MIN;
        } else {
            *value = -(long long)result;
        }
    } else {
        *value = (long long)result;
    }

    return STATUS_OK;
}

Status parse_flag(const char *str, char *flag)
{
    if (str == NULL || flag == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if ((str[0] != '-' && str[0] != '/') ||
        str[1] == '\0' ||
        str[2] != '\0') {
        return STATUS_INVALID_FLAG;
    }

    *flag = str[1];

    return STATUS_OK;
}

Status find_handler(
    char flag,
    const Command *commands,
    size_t count,
    Handler *handler)
{
    size_t i;

    if (commands == NULL || handler == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (count == 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (i = 0; i < count; ++i) {
        if (commands[i].flag == flag) {
            if (commands[i].handler == NULL) {
                return STATUS_INVALID_ARGUMENT;
            }

            *handler = commands[i].handler;
            return STATUS_OK;
        }
    }

    return STATUS_INVALID_FLAG;
}

static Status print_ull(unsigned long long value)
{
    if (printf("%llu\n", value) < 0) {
        return STATUS_OUTPUT_ERROR;
    }

    return STATUS_OK;
}

static Status find_multiples(
    long long x,
    int values[MULTIPLES_LIMIT],
    size_t *count)
{
    int i;

    if (values == NULL || count == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x == 0) {
        return STATUS_OUT_OF_RANGE;
    }

    *count = 0;

    for (i = 1; i <= MULTIPLES_LIMIT; ++i) {
        if (i % x == 0) {
            if (*count >= MULTIPLES_LIMIT) {
                return STATUS_OVERFLOW;
            }

            values[*count] = i;
            ++(*count);
        }
    }

    return STATUS_OK;
}


static Status print_multiples(const int values[MULTIPLES_LIMIT], size_t count)
{
    size_t i;

    if (values == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (count > MULTIPLES_LIMIT) {
        return STATUS_OUT_OF_RANGE;
    }

    if (count == 0) {
        if (printf("There are no multiples in range 1..100\n") < 0) {
            return STATUS_OUTPUT_ERROR;
        }

        return STATUS_OK;
    }



    for (i = 0; i < count; ++i) {
        if (printf("%d", values[i]) < 0) {
            return STATUS_OUTPUT_ERROR;
        }

        if (i + 1 < count) {
            if (printf(" ") < 0) {
                return STATUS_OUTPUT_ERROR;
            }
        }
    }

    if (printf("\n") < 0) {
        return STATUS_OUTPUT_ERROR;
    }

    return STATUS_OK;
}


static Status check_prime(long long x, PrimeResult *result)
{
    long long i;

    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x < 2) {
        *result = PRIME_NEITHER;
        return STATUS_OK;
    }

    for (i = 2; i <= x / i; ++i) {
        if (x % i == 0) {
            *result = PRIME_COMPOSITE;
            return STATUS_OK;
        }
    }

    *result = PRIME_YES;
    return STATUS_OK;
}


static Status print_prime_result(PrimeResult result)
{
    int print_result;

    if (result == PRIME_YES) {
        print_result = printf("Prime\n");
    } else if (result == PRIME_COMPOSITE) {
        print_result = printf("Composite\n");
    } else if (result == PRIME_NEITHER) {
        print_result = printf("Neither prime nor composite\n");
    } else {
        return STATUS_INVALID_ARGUMENT;
    }

    if (print_result < 0) {
        return STATUS_OUTPUT_ERROR;
    }

    return STATUS_OK;
}


static Status get_hex_digits(long long x, char digits[HEX_DIGITS_COUNT], size_t *count)
{
    char reversed[HEX_DIGITS_COUNT];
    unsigned long long value;
    size_t i = 0;
    size_t j;

    if (digits == NULL || count == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x < 0) {
        return STATUS_OUT_OF_RANGE;
    }

    value = (unsigned long long)x;

    if (value == 0) {
        digits[0] = '0';
        *count = 1;
        return STATUS_OK;
    }
    while (value != 0) {
        unsigned int digit;

        if (i >= HEX_DIGITS_COUNT) {
            return STATUS_OVERFLOW;
        }

        digit = (unsigned int)(value % 16ULL);

        if (digit < 10) {
            reversed[i] = (char)('0' + digit);
        } else {
            reversed[i] = (char)('A' + digit - 10);
        }

        ++i;
        value /= 16ULL;
    }

    for (j = 0; j < i; ++j) {
        digits[j] = reversed[i - j - 1];
    }

    *count = i;

    return STATUS_OK;
}


static Status print_hex_digits(const char digits[HEX_DIGITS_COUNT], size_t count)
{
    size_t i;

    if (digits == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (count == 0 || count > HEX_DIGITS_COUNT){
        return STATUS_OUT_OF_RANGE;
    }

    for (i = 0; i < count; ++i) {
        if (printf("%c", digits[i]) < 0) {
            return STATUS_OUTPUT_ERROR;
        }

        if (i + 1 < count) {
            if (printf(" ") < 0) {
                return STATUS_OUTPUT_ERROR;
            }
        }
    }

    if (printf("\n") < 0) {
        return STATUS_OUTPUT_ERROR;
    }

    return STATUS_OK;
}


static Status build_power_table(long long x, unsigned long long table[MAX_POWER_BASE][MAX_POWER_DEGREE])
{
    int base;
    int degree;

    if (table == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x < 1 || x > MAX_POWER_DEGREE) {
        return STATUS_OUT_OF_RANGE;
    }

    for (base = 1; base <= MAX_POWER_BASE; ++base) {
        unsigned long long value = 1;

        for (degree = 1; degree <= x; ++degree) {
            value *= (unsigned long long)base;
            table[base - 1][degree - 1] = value;
        }
    }

    return STATUS_OK;
}


static Status print_power_table(
    unsigned long long table[MAX_POWER_BASE][MAX_POWER_DEGREE],
    long long x)
{
    int base;
    int degree;

    if (table == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x < 1 || x > MAX_POWER_DEGREE) {
        return STATUS_OUT_OF_RANGE;
    }

    for (base = 1; base <= MAX_POWER_BASE; ++base) {
        if (printf("%d:", base) < 0) {
            return STATUS_OUTPUT_ERROR;
        }

        for (degree = 1; degree <= x; ++degree) {
            if (printf(
                    " %llu",
                    table[base - 1][degree - 1]) < 0) {
                return STATUS_OUTPUT_ERROR;
            }
        }

        if (printf("\n") < 0) {
            return STATUS_OUTPUT_ERROR;
        }
    }

    return STATUS_OK;
}


static Status calculate_sum(long long x, unsigned long long *result)
{
    unsigned long long n;
    unsigned long long a;
    unsigned long long b;

    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x < 1) {
        return STATUS_OUT_OF_RANGE;
    }

    n = (unsigned long long)x;

    if (n % 2ULL == 0) {
        a = n / 2ULL;
        b = n + 1ULL;
    } else {
        a = n;
        b = n / 2ULL + 1ULL;
    }

    if (a > ULLONG_MAX / b) {
        return STATUS_OVERFLOW;
    }

    *result = a * b;

    return STATUS_OK;
}


static Status calculate_factorial(long long x, unsigned long long *result)
{
    unsigned long long factorial = 1;
    unsigned long long i;

    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x < 0) {
        return STATUS_OUT_OF_RANGE;
    }

    for (i = 2; i <= (unsigned long long)x; ++i) {
        if (factorial > ULLONG_MAX / i) {
            return STATUS_OVERFLOW;
        }

        factorial *= i;
    }

    *result = factorial;

    return STATUS_OK;
}

Status handle_h(long long x)
{
    int values[MULTIPLES_LIMIT];
    size_t count;
    Status status;

    status = find_multiples(x, values, &count);

    if (status != STATUS_OK) {
        return status;
    }

    return print_multiples(values, count);
}


Status handle_p(long long x)
{
    PrimeResult result;
    Status status;

    status = check_prime(x, &result);

    if (status != STATUS_OK) {
        return status;
    }

    return print_prime_result(result);
}


Status handle_s(long long x)
{
    char digits[HEX_DIGITS_COUNT];
    size_t count;
    Status status;

    status = get_hex_digits(x, digits, &count);

    if (status != STATUS_OK) {
        return status;
    }

    return print_hex_digits(digits, count);
}


Status handle_e(long long x)
{
    unsigned long long table[MAX_POWER_BASE][MAX_POWER_DEGREE] = {{0}};
    Status status;

    status = build_power_table(x, table);

    if (status != STATUS_OK) {
        return status;
    }

    return print_power_table(table, x);
}


Status handle_a(long long x)
{
    unsigned long long result;
    Status status;

    status = calculate_sum(x, &result);

    if (status != STATUS_OK) {
        return status;
    }

    return print_ull(result);
}

Status handle_f(long long x)
{
    unsigned long long result;
    Status status;

    status = calculate_factorial(x, &result);

    if (status != STATUS_OK) {
        return status;
    }

    return print_ull(result);
}

