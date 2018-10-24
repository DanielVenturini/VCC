#include "stack.h"

// function to create a stack of given capacity. It initializes size of
// stack as 0
Stack* createStack(unsigned capacity)
{
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (char *) malloc(stack->capacity*sizeof(char));
    return stack;
}

// Stack is full when top is equal to the last index
int isFull(Stack* stack)
{   return stack->top == stack->capacity - 1; }

// Stack is empty when top is equal to -1
int isEmpty(Stack* stack)
{   return stack->top == -1;  }

void realocaPilha(Stack *stack) {
    stack->array = (char *) realloc(stack->array, stack->capacity+MAX_PILHA);
    stack->capacity += MAX_PILHA;
}

// Function to add an item to stack.  It increases top by 1
void push(Stack* stack, char item)
{
    if (isFull(stack))
        realocaPilha(stack);

    stack->array[++stack->top] = item;
    //printf("%s pushed to stack\n", item);
}

// Function to remove an item from stack.  It decreases top by 1
char pop(Stack* stack)
{
    if (isEmpty(stack))
        return 0;

    return stack->array[stack->top--];
}