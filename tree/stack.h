/* https://www.geeksforgeeks.org/stack-data-structure-introduction-program/ */

// C program for array implementation of stack
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _STACK_H
#define _STACK_H

#define MAX_PILHA 1000

// A structure to represent a stack
typedef struct
{
    int top;
    unsigned capacity;
    char *array;

} Stack;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

// function to create a stack of given capacity. It initializes size of
// stack as 0
Stack* createStack(unsigned capacity);

// Stack is full when top is equal to the last index
int isFull(Stack* stack);

// Stack is empty when top is equal to -1
int isEmpty(Stack* stack);

// Function to add an item to stack.  It increases top by 1
void push(Stack* stack, char item);

// Function to remove an item from stack.  It decreases top by 1
char pop(Stack* stack);

#endif