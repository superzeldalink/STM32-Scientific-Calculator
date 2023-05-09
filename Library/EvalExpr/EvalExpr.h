#ifndef EXPRESSION_EVALUATION_H
#define EXPRESSION_EVALUATION_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

#define MAX 100 // maximum size of stack
#define TOLERANCE 1e-9
#define E M_E

double answer = 0;
double varSto[4];

// stack structure
struct stack {
	int top;
	double items[MAX];
};

// evaluate a math expression from string
double evaluate(char *exp, uint8_t size, uint8_t* errorCode);
double limit(char *exp, double x0, char size, uint8_t *error);
double derivative(char *exp, double x, char size, uint8_t *error);
double integrate(char *exp, double a, double b, double tol, uint8_t maxDepth, char size, uint8_t *error);
double ExpSolve(char *exp, char size, uint8_t *error);

typedef struct {
    int sign;
    long unsigned num;
    long unsigned den;
} Fraction;

Fraction to_fraction(double x);

#endif /* EXPRESSION_EVALUATION_H */
