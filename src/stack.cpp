#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "../include/color_print.h"
#include "stack.h"

const stack_elem_t STACK_POISON_ELEM = NULL;

const double CANARY_VALUE        = (0xBAD);
const double CANARY_BUFFER_VALUE = (0xEDA);

enum StackCondition stack_ctor (struct stack_t* stack, int capacity ON_DBG(, const char* file, int line, const char* func))
{
    assert(stack);

    ON_CNR_PRTCT ( stack->canary_stack_1 = CANARY_VALUE;
                   stack->canary_stack_2 = CANARY_VALUE; )

    STACK_ASSERT(stack, file, line, func);

    stack->size = 0;

    stack->data = ON_CNR_PRTCT(1 +) (stack_elem_t*) calloc((size_t) capacity ON_CNR_PRTCT(+ 2), sizeof(stack->data[0]));

    stack->capacity = capacity;

    ON_CNR_PRTCT ( memcpy(&stack->data[-1],              &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE));
                   memcpy(&stack->data[stack->capacity], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)); )

    for (int i = 0; i < stack->capacity; i++)
            stack->data[stack->size + i] = STACK_POISON_ELEM;

    ON_HASH ( calculate_hash(stack); )

    int error_code = stack_error (stack);
    if ( error_code )
        return (StackCondition) error_code;

    ON_DBG ( STACK_ASSERT(stack, file, line, func); )

    return STACK_IS_OK;
}

int stack_dtor (struct stack_t* stack)
{
    for (int i = 0; i < stack->capacity; i++)
        stack->data[i] = STACK_POISON_ELEM;

    free(stack->data ON_CNR_PRTCT (- 1)); stack->data = NULL;

    stack->size = 0;

    stack->capacity = 0;

    return 0;
}

int increasing_capacity (struct stack_t* stack ON_DBG(, const char* file, int line, const char* func))
{
    if (stack->size >= stack->capacity)
    {
        stack->capacity *= 2;

        stack->data = ON_CNR_PRTCT(1 +) (stack_elem_t*) realloc (stack->data ON_CNR_PRTCT(- 1), (size_t) (stack->capacity ON_CNR_PRTCT(+ 2)) * sizeof(stack->data[0]));

        for (int i = 0; i < stack->size; i++)
            stack->data[stack->size + i] = STACK_POISON_ELEM;

        ON_CNR_PRTCT ( memcpy(&stack->data[stack->capacity], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)); )

        ON_HASH ( calculate_hash(stack); )

        ON_DBG ( STACK_ASSERT(stack, file, line, func); )
    }

    return 0;
}

int stack_push (struct stack_t* stack, stack_elem_t elem ON_DBG(, const char* file, int line, const char* func))
{
    STACK_ASSERT(stack, file, line, func);

    increasing_capacity (stack ON_DBG(, file, line, func));

    stack->data[stack->size++] = elem;

    ON_HASH ( calculate_hash(stack); )

    ON_DBG ( STACK_ASSERT(stack, file, line, func); )

    return 0;
}

ON_HASH ( int calculate_hash (struct stack_t* stack)
{
    stack->hash_b  = hash_djb2((const char*)(stack->data ON_CNR_PRTCT(- 1)), (size_t)stack->capacity * sizeof (stack->data[0]));
    stack->hash_st = 0;
    stack->hash_st = hash_djb2((const char*)stack, sizeof(*stack));

    return 0;
} )

int stack_pop (struct stack_t* stack, stack_elem_t* x ON_DBG(, const char* file, int line, const char* func))
{
    ON_DBG ( STACK_ASSERT(stack, file, line, func); )

    if (stack->size <= 0)
        return STACK_BAD_SIZE;

    *x = stack->data[stack->size - 1];
    stack->data[--stack->size] = STACK_POISON_ELEM;

    ON_HASH ( calculate_hash(stack); )

    return 0;
}

int stack_dump (struct stack_t* stack, const char* file, int line, const char* func)
{
    printf(PURPLE_TEXT("\nstack_t [%p] called from %s, name \"%s\" born at %s:%d:\n"), stack, file, "stack", func, line);

    if (stack == NULL)
    {
        printf("stack = " BLUE_TEXT("[%p]") ", this is all the information I can give\n\n", stack);
        return 0;
    }
    ON_HASH      ( printf(  "hash_st = " BLUE_TEXT("%x") "\n", (unsigned int)stack->hash_st);
                   printf(  "hash_b  = " BLUE_TEXT("%x") "\n", (unsigned int)stack->hash_b ); )
    ON_CNR_PRTCT ( printf("\ncanary_1 in stack   = " BLUE_TEXT("0x%x") "\n\n" , (unsigned int)stack->canary_stack_1); )
                   printf(  "stack->capacity     = " BLUE_TEXT("%d") "\n",      stack->capacity);
                   printf(  "stack->size         = " BLUE_TEXT("%d") "\n",      stack->size);
                   printf(  "stack->data address = " BLUE_TEXT("[%p]") "\n\n",  stack->data);
    ON_CNR_PRTCT ( printf(  "canary_2 in stack   = " BLUE_TEXT("0x%x") "\n\n" , (unsigned int)stack->canary_stack_2); )

    if (stack->data == NULL)
    {
        printf("stack->data = " BLUE_TEXT("[%p]") ", this is all the information I can give", stack->data);
        return 0;
    }

    for (int i = 0; i < stack->capacity; i++)
    {
        if (memcmp(&stack->data[i], &STACK_POISON_ELEM, sizeof(STACK_POISON_ELEM)) == 0)
            printf("stack->data[%d] = " BLUE_TEXT("%p (POISON)") "\n", i, stack->data[i]);
        else
            printf("stack->data[%d] = " BLUE_TEXT("%p") "\n", i, stack->data[i]);
    }

    ON_CNR_PRTCT ( printf("\ncanary_3 in buffer = " BLUE_TEXT("0x%x") "\n\n", (unsigned int)stack->data[-1]);
                   printf("canary_4 in buffer = "   BLUE_TEXT("0x%x") "\n\n", (unsigned int)stack->data[stack->capacity]); )

    return 0;
}

int stack_error (struct stack_t* stack)
{
    int stack_err = 0;

    if (stack == NULL)
        stack_err |= STACK_IS_NULL;

    if (stack->data == NULL)
        stack_err |= STACK_DATA_IS_NULL;

    if (stack->size < 0)
        stack_err |= STACK_BAD_SIZE;

    if ((stack->size) > (stack->capacity))
        stack_err |= STACK_SIZE_IS_LESS_CAPACITY;

    ON_CNR_PRTCT ( if (memcmp(&stack->canary_stack_1, &CANARY_VALUE, sizeof(CANARY_VALUE)) ||
                       memcmp(&stack->canary_stack_2, &CANARY_VALUE, sizeof(CANARY_VALUE)))
                       stack_err |= STACK_CANARY_PROBLEM;

                   if (memcmp(&stack->data[-1],              &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)) ||
                       memcmp(&stack->data[stack->capacity], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)))
                       stack_err |= STACK_DATA_CANARY_PROBLEM; )

    ON_HASH    ( hash_t old_hash_st = stack->hash_st;
                 stack->hash_st = 0;
                 stack->hash_st = hash_djb2((const char*)stack, sizeof(*stack));

                 if (old_hash_st != stack->hash_st)
                     stack_err |= STACK_HASH_PROBLEM;

                 stack->hash_st = old_hash_st;

                 hash_t old_hash_b = stack->hash_b;
                 stack->hash_b = 0;
                 stack->hash_b  = hash_djb2((const char*)(stack->data ON_CNR_PRTCT(- 1)), (size_t)stack->capacity * sizeof (stack->data[0]));

                 if (old_hash_b != stack->hash_b)
                     stack_err |= STACK_DATA_HASH_PROBLEM;

                 stack->hash_b = old_hash_b; )

    return stack_err;
}

int error_code_output (int stack_err)
{
    printf(RED_TEXT("ERROR IN STACK: "));

    convert_to_binary(stack_err);

    switch (stack_err)
    {
        case STACK_IS_NULL:
            printf(RED_TEXT(" - STACK_IS_NULL"));
            break;
        case STACK_DATA_IS_NULL:
            printf(RED_TEXT(" - STACK_DATA_IS_NULL"));
            break;
        case STACK_BAD_SIZE:
            printf(RED_TEXT(" - STACK_BAD_SIZE"));
            break;
        case STACK_SIZE_IS_LESS_CAPACITY:
            printf(RED_TEXT(" - STACK_SIZE_IS_LESS_CAPACITY"));
            break;
        case STACK_CANARY_PROBLEM:
            printf(RED_TEXT(" - STACK_CANARY_PROBLEM"));
            break;
        case STACK_DATA_CANARY_PROBLEM:
            printf(RED_TEXT(" - STACK_DATA_CANARY_PROBLEM"));
            break;
        case STACK_HASH_PROBLEM:
            printf(RED_TEXT(" - STACK_HASH_PROBLEM"));
            break;
        case STACK_DATA_HASH_PROBLEM:
            printf(RED_TEXT(" - STACK_DATA_HASH_PROBLEM"));
            break;
        default:
            printf(RED_TEXT(" - UNKNOWN ERROR IN STACK"));
    }

    printf("\n");

    return 0;
}

int convert_to_binary (int n)
{
    if (n > 1) convert_to_binary(n / 2);

    printf(RED_TEXT("%d"), (int) n % 2);

    return 0;
}

int convert_to_binary_upd (unsigned n)
{
    for (int i = 0; i < 32; i++)
        {
        printf ("%c", (int)((n & 1) + '0'));
        n >>= 1;
        }

    return 0;
}

stack_elem_t stack_look (struct stack_t* stack, int n)
{
    if (!(0 <= n && n < stack->size))
        return NULL;

    return stack->data[n];
}

int stack_assert (struct stack_t* stack, const char* file, int line, const char* func)
{
    if (!stack_error(stack))
        return 0;

    printf_place_info(file, line, func, "STACK_ASSERT");

    fprintf(stderr, RED_TEXT("STACK IS NOT OKAY:\n"));

    error_code_output(stack_error(stack));

    if (stack == NULL)
        fprintf(stderr, YELLOW_TEXT("stack = NULL\n"));

    if (stack->data == NULL)
        fprintf(stderr, YELLOW_TEXT("stack->data = NULL\n"));

    if ((stack->size) > (stack->capacity))
        fprintf(stderr, YELLOW_TEXT("stack->size = %d | stack->capacity = %d\n"), stack->size, stack->capacity);

    ON_CNR_PRTCT ( if (memcmp(&stack->canary_stack_1, &CANARY_VALUE, sizeof(CANARY_VALUE)) ||
                       memcmp(&stack->canary_stack_2, &CANARY_VALUE, sizeof(CANARY_VALUE)))
                       fprintf(stderr, YELLOW_TEXT("stack->canary_stack_1 = %x | stack->canary_stack_2 = %x\n"),
                              (unsigned int)stack->canary_stack_1, (unsigned int)stack->canary_stack_2);

                   if (memcmp(&stack->data[-1], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)) ||
                       memcmp(&stack->data[stack->capacity], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)))
                       fprintf(stderr, YELLOW_TEXT("canary_buffer_3 = %x | canary_buffer_4 = %x\n"),
                              (unsigned int)stack->data[-1], (unsigned int)stack->data[stack->capacity]); )

    ON_HASH    ( hash_t old_hash_st = stack->hash_st;
                 stack->hash_st = 0;
                 stack->hash_st = hash_djb2((char*)stack, sizeof(*stack));

                 if (old_hash_st != stack->hash_st)
                     fprintf(stderr, YELLOW_TEXT("old_hash_st = %x | stack->hash_st = %x\n"),
                             (unsigned int)old_hash_st, (unsigned int)stack->hash_st);

                 stack->hash_st = old_hash_st;

                 hash_t old_hash_b = stack->hash_b;
                 stack->hash_b = 0;
                 stack->hash_b  = hash_djb2((char*)(stack->data ON_CNR_PRTCT(- 1)), (size_t)stack->capacity * sizeof (stack->data[0]));

                 if (old_hash_b != stack->hash_b)
                     fprintf(stderr, YELLOW_TEXT("old_hash_b = %x | stack->hash_b = %x\n"),
                             (unsigned int)old_hash_b, (unsigned int)stack->hash_b);

                 stack->hash_b = old_hash_b; )

    assert(0);

    return 0;
}

void printf_place_info (const char* file, int line, const char* func, const char* string)
{
    printf(PURPLE_TEXT("called from %s, name \"%s\" born at %s:%d:\n"),  file, string, func, line);
}

hash_t hash_djb2 (const char* str, size_t size_of_str)
{
    hash_t hash = 5381;

    for (size_t i = 0; i < size_of_str; i++)
    {
        hash_t a = (hash_t) *str++;
        hash = ((hash << 5) + hash) + a; // or hash * 33 + a
    }

    return hash;
}
