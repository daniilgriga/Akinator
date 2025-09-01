#ifndef STACK_H_
#define STACK_H_

// #define CANARY_PROTECTION
// #define DEBUG
// #define HASH

#include <stdbool.h>
#include "head.h"

#ifdef CANARY_PROTECTION
    #define ON_CNR_PRTCT(...) __VA_ARGS__
#else
    #define ON_CNR_PRTCT(...)
#endif

#ifdef DEBUG
    #define ON_DBG(...) __VA_ARGS__
#else
    #define ON_DBG(...)
#endif

#ifdef HASH
    #define ON_HASH(...) __VA_ARGS__
#else
    #define ON_HASH(...)
#endif

#ifdef DEBUG
    #define STACK_ASSERT(...) stack_assert(__VA_ARGS__)
#else
    #define STACK_ASSERT(...)
#endif

typedef unsigned long hash_t;

typedef struct Node_t* stack_elem_t;

#pragma GCC diagnostic ignored "-Wlarger-than=8192"
#pragma GCC diagnostic ignored "-Wstack-protector"

struct stack_t
{
    ON_CNR_PRTCT (double canary_stack_1;)

    stack_elem_t* data;
    int size;
    int capacity;

    ON_HASH ( hash_t hash_st;
              hash_t hash_b;  )

    ON_CNR_PRTCT (double canary_stack_2;)
};

ON_DBG (struct test_str
{
    char executioner_1[1];
    struct stack_t stack;
    char executioner_2[1];
};  )

enum StackCondition
{
    STACK_IS_OK  = 0,
    STACK_IS_BAD = 1
};

enum errors
{
    STACK_OK                    = 0,
    STACK_IS_NULL               = 1,
    STACK_DATA_IS_NULL          = 2,
    STACK_BAD_SIZE              = 4,
    STACK_SIZE_IS_LESS_CAPACITY = 8,
    STACK_CANARY_PROBLEM        = 16,
    STACK_DATA_CANARY_PROBLEM   = 32,
    STACK_HASH_PROBLEM          = 64,
    STACK_DATA_HASH_PROBLEM     = 128
};

extern const stack_elem_t STACK_POISON_ELEM;

extern const double CANARY_VALUE;
extern const double CANARY_BUFFER_VALUE;

int stack_error (struct stack_t* stack);

int error_code_output (int err);

int convert_to_binary (int n);

enum StackCondition stack_ctor (struct stack_t* stack, int capacity ON_DBG(, const char* file, int line, const char* func));

int stack_assert (struct stack_t* stack, const char* file, int line, const char* func);

int stack_dtor (struct stack_t* stack);

int stack_push (struct stack_t* stack, stack_elem_t elem ON_DBG(, const char* file, int line, const char* func));

int stack_pop (struct stack_t* stack, stack_elem_t* x ON_DBG(, const char* file, int line, const char* func));

int stack_dump (struct stack_t* stack, const char* file, int line, const char* func);

bool stack_ok (const struct stack_t* stack);

int increasing_capacity (struct stack_t* stack ON_DBG(, const char* file, int line, const char* func));

stack_elem_t stack_look (struct stack_t* stack, int n);

void printf_place_info (const char* file, int line, const char* func, const char* string);

hash_t hash_djb2(const char* str, size_t size_of_stack);

int convert_to_binary_upd (unsigned n);

int calculate_hash (struct stack_t* stack);

#endif
