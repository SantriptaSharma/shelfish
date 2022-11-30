#ifndef LOG_H_GUARD
#define LOG_H_GUARD
#define log(x) printf(x); printf("\n")
#define logf(x, ...) printf(x, __VA_ARGS__); printf("\n")
#define error(x) printf("\033[1;31m"); printf(x); printf("\033[0m"); printf("\n")
#define errorf(x, ...) printf("\033[1;31m"); printf(x, __VA_ARGS__); printf("\033[0m"); printf("\n")
#endif