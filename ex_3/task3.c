#include "task3.h"

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    EQUATION_NO_SOLUTIONS,
    EQUATION_NO_REAL_ROOTS,
    EQUATION_ONE_ROOT,
    EQUATION_TWO_ROOTS,
    EQUATION_INFINITE_ROOTS
} EquationType;

typedef struct {
    EquationType type;
    double x1;
    double x2;
} EquationSolution;

typedef struct {
    double a;
    double b;
    double c;
    EquationSolution solution;
} EquationResult;

static int is_equal(double a, double b, double epsilon);

static Status solve_equation(
    double a,
    double b,
    double c,
    double epsilon,
    EquationSolution *solution
);

static int same_coefficients(
    const EquationResult *result,
    double a,
    double b,
    double c,
    double epsilon
);

static Status calculate_permutations(
    double a,
    double b,
    double c,
    double epsilon,
    EquationResult results[6],
    size_t *count
);

static Status print_equation_results(
    const EquationResult results[6],
    size_t count
);

static Status check_multiple(
    long long first,
    long long second,
    int *result
);

static Status print_multiple_result(
    long long first,
    long long second,
    int result
);

static Status check_right_triangle(
    double a,
    double b,
    double c,
    double epsilon,
    int *result
);

static Status print_triangle_result(int result);
static int is_valid_epsilon(double epsilon);

static int is_valid_epsilon(double epsilon)
{
    return isfinite(epsilon) &&
           epsilon > 0.0 &&
           epsilon < 1.0;
}

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


Status parse_double(const char *str, double *value)
{
    char *end;
    double result;

    if (str == NULL || value == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (str[0] == '\0' || isspace((unsigned char)str[0])) {
        return STATUS_INVALID_NUMBER;
    }

    end = NULL;
    result = strtod(str, &end);

    if (end == str || end == NULL || *end != '\0') {
        return STATUS_INVALID_NUMBER;
    }

    if (!isfinite(result)) {
        return STATUS_OVERFLOW;
    }

    *value = result;

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


Status find_command(
    char flag,
    const Command *commands,
    size_t count,
    const Command **command)
{
    size_t i;

    if (commands == NULL || command == NULL || count == 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (i = 0; i < count; ++i) {
        if (commands[i].flag == flag) {
            if (commands[i].handler == NULL) {
                return STATUS_INVALID_ARGUMENT;
            }

            *command = &commands[i];
            return STATUS_OK;
        }
    }

    return STATUS_INVALID_FLAG;
}


static int is_equal(double a, double b, double epsilon)
{
    if (!is_valid_epsilon(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }

    return fabs(a - b) <= epsilon;
}


static Status solve_equation(
    double a,
    double b,
    double c,
    double epsilon,
    EquationSolution *solution)
{
    double discriminant;
    double denominator;
    double root;

    if (solution == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }
    if (!is_valid_epsilon(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }

    if (!(epsilon > 0.0) || !isfinite(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }

    if (is_equal(a, 0.0, epsilon)) {
        if (is_equal(b, 0.0, epsilon)) {
            solution->type = is_equal(c, 0.0, epsilon)
                ? EQUATION_INFINITE_ROOTS
                : EQUATION_NO_SOLUTIONS;

            solution->x1 = 0.0;
            solution->x2 = 0.0;

            return STATUS_OK;
        }

        root = -c / b;

        if (!isfinite(root)) {
            return STATUS_OVERFLOW;
        }

        solution->type = EQUATION_ONE_ROOT;
        solution->x1 = root;
        solution->x2 = root;

        return STATUS_OK;
    }

    discriminant = b * b - 4.0 * a * c;

    if (!isfinite(discriminant)) {
        return STATUS_OVERFLOW;
    }

    if (discriminant < -epsilon) {
        solution->type = EQUATION_NO_REAL_ROOTS;
        solution->x1 = 0.0;
        solution->x2 = 0.0;

        return STATUS_OK;
    }

    denominator = 2.0 * a;

    if (!isfinite(denominator)) {
        return STATUS_OVERFLOW;
    }

    if (is_equal(discriminant, 0.0, epsilon)) {
        root = -b / denominator;

        if (!isfinite(root)) {
            return STATUS_OVERFLOW;
        }

        solution->type = EQUATION_ONE_ROOT;
        solution->x1 = root;
        solution->x2 = root;

        return STATUS_OK;
    }

    root = sqrt(discriminant);

    solution->x1 = (-b + root) / denominator;
    solution->x2 = (-b - root) / denominator;

    if (!isfinite(solution->x1) ||
        !isfinite(solution->x2)) {
        return STATUS_OVERFLOW;
    }

    solution->type = EQUATION_TWO_ROOTS;

    return STATUS_OK;
}


static int same_coefficients(
    const EquationResult *result,
    double a,
    double b,
    double c,
    double epsilon)
{
    return is_equal(result->a, a, epsilon) &&
           is_equal(result->b, b, epsilon) &&
           is_equal(result->c, c, epsilon);
}


static Status calculate_permutations(
    double a,
    double b,
    double c,
    double epsilon,
    EquationResult results[6],
    size_t *count)
{
    const double values[3] = {a, b, c};

    const size_t permutations[6][3] = {
        {0, 1, 2},
        {0, 2, 1},
        {1, 0, 2},
        {1, 2, 0},
        {2, 0, 1},
        {2, 1, 0}
    };

    size_t i;
    size_t j;

    if (results == NULL || count == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }
    if (!is_valid_epsilon(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }
    if (!(epsilon > 0.0) || !isfinite(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }

    *count = 0;

    for (i = 0; i < 6; ++i) {
        double pa = values[permutations[i][0]];
        double pb = values[permutations[i][1]];
        double pc = values[permutations[i][2]];

        int duplicate = 0;
        Status status;

        for (j = 0; j < *count; ++j) {
            if (same_coefficients(
                    &results[j],
                    pa,
                    pb,
                    pc,
                    epsilon)) {
                duplicate = 1;
                break;
            }
        }

        if (duplicate) {
            continue;
        }

        results[*count].a = pa;
        results[*count].b = pb;
        results[*count].c = pc;

        status = solve_equation(
            pa,
            pb,
            pc,
            epsilon,
            &results[*count].solution
        );

        if (status != STATUS_OK) {
            return status;
        }

        ++(*count);
    }

    return STATUS_OK;
}


static Status print_equation_results(
    const EquationResult results[6],
    size_t count)
{
    size_t i;

    if (results == NULL || count == 0 || count > 6) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (i = 0; i < count; ++i) {
        int printed;

        printed = printf(
            "a=%.15g, b=%.15g, c=%.15g: ",
            results[i].a,
            results[i].b,
            results[i].c
        );

        if (printed < 0) {
            return STATUS_OUTPUT_ERROR;
        }

        switch (results[i].solution.type) {
        case EQUATION_NO_SOLUTIONS:
            printed = printf("no solutions\n");
            break;        
        
        case EQUATION_NO_REAL_ROOTS:
            printed = printf("no real roots\n");
            break;

        case EQUATION_ONE_ROOT:
            printed = printf(
                "x=%.15g\n",
                results[i].solution.x1
            );
            break;

        case EQUATION_TWO_ROOTS:
            printed = printf(
                "x1=%.15g, x2=%.15g\n",
                results[i].solution.x1,
                results[i].solution.x2
            );
            break;

        case EQUATION_INFINITE_ROOTS:
            printed = printf("infinitely many roots\n");
            break;

        default:
            return STATUS_INVALID_ARGUMENT;
        }

        if (printed < 0) {
            return STATUS_OUTPUT_ERROR;
        }
    }

    return STATUS_OK;
}


static Status check_multiple(
    long long first,
    long long second,
    int *result)
{
    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (first == 0 || second == 0) {
        return STATUS_OUT_OF_RANGE;
    }

    /*
     * LLONG_MIN % -1 вызывает переполнение
     * при выполнении целочисленного деления.
     * Математически число при этом делится на -1.
     */
    if (first == LLONG_MIN && second == -1) {
        *result = 1;
        return STATUS_OK;
    }

    *result = (first % second == 0);

    return STATUS_OK;
}


static Status print_multiple_result(
    long long first,
    long long second,
    int result)
{
    int printed;

    if (result != 0 && result != 1) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (result) {
        printed = printf(
            "%lld is divisible by %lld\n",
            first,
            second
        );
    } else {
        printed = printf(
            "%lld is not divisible by %lld\n",
            first,
            second
        );
    }

    if (printed < 0) {
        return STATUS_OUTPUT_ERROR;
    }

    return STATUS_OK;
}


static Status check_right_triangle(
    double a,
    double b,
    double c,
    double epsilon,
    int *result)
{
    double a2;
    double b2;
    double c2;

    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }
    if (!is_valid_epsilon(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }
    if (!(epsilon > 0.0) || !isfinite(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }

    if (a <= 0.0 || b <= 0.0 || c <= 0.0) {
        return STATUS_OUT_OF_RANGE;
    }

    a2 = a * a;
    b2 = b * b;
    c2 = c * c;

    if (!isfinite(a2) ||
        !isfinite(b2) ||
        !isfinite(c2)) {
        return STATUS_OVERFLOW;
    }

    if (!isfinite(a2 + b2) ||
        !isfinite(a2 + c2) ||
        !isfinite(b2 + c2)) {
        return STATUS_OVERFLOW;
    }

    *result =
        is_equal(a2 + b2, c2, epsilon) ||
        is_equal(a2 + c2, b2, epsilon) ||
        is_equal(b2 + c2, a2, epsilon);

    return STATUS_OK;
}


static Status print_triangle_result(int result)
{
    int printed;

    if (result != 0 && result != 1) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (result) {
        printed = printf(
            "The sides can form a right triangle\n"
        );
    } else {
        printed = printf(
            "The sides cannot form a right triangle\n"
        );
    }

    if (printed < 0) {
        return STATUS_OUTPUT_ERROR;
    }

    return STATUS_OK;
}


Status handle_q(char *args[])
{
    double epsilon;
    double a;
    double b;
    double c;

    EquationResult results[6];
    size_t count;

    Status status;

    if (args == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    status = parse_double(args[0], &epsilon);

    if (status != STATUS_OK) {
        return status;
    }
    if (!is_valid_epsilon(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }
    if (!(epsilon > 0.0)) {
        return STATUS_OUT_OF_RANGE;
    }

    status = parse_double(args[1], &a);

    if (status != STATUS_OK) {
        return status;
    }

    status = parse_double(args[2], &b);

    if (status != STATUS_OK) {
        return status;
    }

    status = parse_double(args[3], &c);

    if (status != STATUS_OK) {
        return status;
    }

    status = calculate_permutations(
        a,
        b,
        c,
        epsilon,
        results,
        &count
    );

    if (status != STATUS_OK) {
        return status;
    }

    return print_equation_results(results, count);
}


Status handle_m(char *args[])
{
    long long first;
    long long second;
    int result;

    Status status;

    if (args == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    status = parse_ll(args[0], &first);

    if (status != STATUS_OK) {
        return status;
    }

    status = parse_ll(args[1], &second);

    if (status != STATUS_OK) {
        return status;
    }

    status = check_multiple(
        first,
        second,
        &result
    );

    if (status != STATUS_OK) {
        return status;
    }

    return print_multiple_result(
        first,
        second,
        result
    );
}


Status handle_t(char *args[])
{
    double epsilon;
    double a;
    double b;
    double c;

    int result;
    Status status;

    if (args == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    status = parse_double(args[0], &epsilon);

    if (status != STATUS_OK) {
        return status;
    }
    if (!is_valid_epsilon(epsilon)) {
        return STATUS_OUT_OF_RANGE;
    }
    if (!(epsilon > 0.0)) {
        return STATUS_OUT_OF_RANGE;
    }

    status = parse_double(args[1], &a);

    if (status != STATUS_OK) {
        return status;
    }

    status = parse_double(args[2], &b);

    if (status != STATUS_OK) {
        return status;
    }

    status = parse_double(args[3], &c);

    if (status != STATUS_OK) {
        return status;
    }

    status = check_right_triangle(
        a,
        b,
        c,
        epsilon,
        &result
    );

    if (status != STATUS_OK) {
        return status;
    }

    return print_triangle_result(result);
}