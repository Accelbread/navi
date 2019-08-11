#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <argp.h>

static char doc[] = "Prints OUTPUT to stdout every TIME seconds.";
static char args_doc[] = "TIME OUTPUT";

static struct argp_option options[] = {
  {"count", 'c', "COUNT", 0, "Print COUNT times then exit.", 0},
  { 0 }
};

struct args {
    int time;
    char *output;
    int count;
};

static bool parse_int_opt(char const *str, int *val) {
    long val_l;
    char *end;
    val_l = strtol(str, &end, 10);
    // if parse did not reach end of string or if arg is empty
    if (*end != '\0' || end == str) {
        return false;
    }
    // if parsed value does not fit in int
    if (val_l < INT_MIN || val_l > INT_MAX) {
        return false;
    }
    *val = (int) val_l;
    return true;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct args *args = state->input;
    switch (key) {
        case 'c':
            if (!parse_int_opt(arg, &args->count)) {
                argp_usage(state);
            }
            break;
        case ARGP_KEY_ARG:
            switch (state->arg_num) {
                // TIME
                case 0:
                    if (!parse_int_opt(arg, &args->time)) {
                        argp_usage(state);
                    }
                    break;
                // OUTPUT
                case 1:
                    args->output = arg;
                    break;
                // Too many args
                default:
                    argp_usage(state);
            }
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[]) {
    struct args args = {.count = -1};
    argp_parse(&argp, argc, argv, 0, 0, &args);

    // Allow negative count to loop infinitely
    while (args.count != 0) {
        printf("%s\n", args.output);
        fflush(stdout);
        sleep(args.time);
        if (args.count > 0) {
            args.count--;
        }
    }
}
