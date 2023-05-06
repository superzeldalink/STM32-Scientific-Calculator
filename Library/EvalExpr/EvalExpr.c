#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "EvalExpr.h"
#include "../KeyPad/KeyPad.h"

// Variable Storage
void InitSto() {
	for (int i = 0; i < 4; i++) {
		varSto[i] = 0;
	}
}

// Get value
double GetVar(char c) {
	switch(c){
	case ANSWER: return varSto[0];
	case X: return varSto[1];
	case Y: return varSto[2];
	case Z: return varSto[3];
	}

	return 0;
}

// Set value
void SetVar(char c, double val) {
	switch(c){
	case ANSWER: varSto[0] = val; break;
	case X: varSto[1] = val; break;
	case Y: varSto[2] = val; break;
	case Z: varSto[3] = val; break;
	}
}

// push an item into stack
void push(struct stack *s, double value, uint8_t *error) {
    if (s->top == MAX - 1) {
        // Stack is full
        *error = 1;
        return;
    }
    s->items[++(s->top)] = value;
}

// pop an item from stack
double pop(struct stack *s, uint8_t *error) {
    if (s->top == -1) {
        // Stack is empty
        *error = 2;
        return 0;
    }
    return s->items[(s->top)--];
}

// check if a character is an operator
int is_operator(char c) {
    return c == PLUS || c == MINUS || c == MULTIPLY || c == DIVIDE || c == EXPONENT || c == SQRT || c == XRT ||
           c == FACTORIAL || c == SINE || c == COSINE || c == TANGENT ||
		   c == LN || c == LOG || c == LOGX;
}

int is_variable(char c) {
	return c == ANSWER || c == X || c == Y || c == Z;
}

// check the precedence of operators
int precedence(char op) {
    switch (op) {
        case PLUS:
        case MINUS:
            return 1;
        case MULTIPLY:
        case DIVIDE:
            return 2;
        case EXPONENT:
        case XRT:
            return 3;
        case FACTORIAL:
        	return 4;
        case SQRT:
        case SINE:
        case COSINE:
        case TANGENT:
        case LN:
        case LOG:
        case LOGX:
            return 5;
        default:
            return 0;
    }
}

// perform arithmetic operation
double operate(double x, double y, char op, uint8_t *error) {
    switch (op) {
        case PLUS:
            return x + y;
        case MINUS:
            return x - y;
        case MULTIPLY:
            return x * y;
        case DIVIDE:
            if (y == 0) {
                // Division by zero
                if(*error > 2 || *error == 0) *error = 4;
                return 0;
            }
            return x / y;
        case EXPONENT:
            return pow(x, y);
        case SQRT:
        	return sqrt(x);
        case XRT:
        	return pow (y, 1.0/x);
        case SINE:
            return sin(x);
        case COSINE:
            return cos(x);
        case TANGENT:
            return tan(x);
        case LN:
        	return log10(x)/log10(E);
        case LOG:
        	return log10(x);
        case LOGX:
            return log10(y)/log10(x);
        case FACTORIAL:
        	return tgamma(x + 1);
        default:
            // Invalid operator
        	return 0;
    }
}

double itod(const char *digits, char* max_addr, int* endi) {
    double sign = 1.0f, value = 0.0f, decimal = 0.0f;
    int has_decimal = 0, digits_before_decimal = 0;

    // Check for optional sign
    if (digits[0] == MINUS) {
        sign = -1.0f;
        digits_before_decimal++;
    }

	int i = digits_before_decimal;

    // Parse integer part
    while (digits[i] <= DOT && (digits + i) < max_addr) {
        if (digits[i] == DOT) {
            has_decimal = 1;
		    i++;
            continue;
        }
        if (has_decimal) {
            decimal = decimal * 10.0f + digits[i];
        } else {
            value = value * 10.0f + digits[i];
            digits_before_decimal++;
        }
		i++;
    }

	*endi = i;

    // Calculate final value
    if (digits_before_decimal > 0) {
        value += decimal / (double)pow(10.0f, i - digits_before_decimal - 1);
        return sign * value;
    }

    // Error: no digits found
	*endi = 0;
    return 0.0f;
}

void calculate(struct stack *operators, struct stack *operands, uint8_t* errorCode) {
    // pop two operands and one operator
    char op = pop(operators, errorCode);
    double y = 0;
    if ((op < SINE && op != FACTORIAL && op != SQRT) || op == LOGX)
        y = pop(operands, errorCode);
    double x = pop(operands, errorCode);

    // evaluate the expression x op y and push the result into operands stack
    push(operands, operate(x, y, op, errorCode), errorCode);
}

// evaluate a math expression from array
double ExpEvaluate(char *exp, uint8_t size, uint8_t* errorCode) {
    // create two stacks: one for operands and one for operators
    struct stack operands = {-1};
    struct stack operators = {-1};

    // track the number of opening and closing parentheses
    int numOpenBrackets = 0;
    int numCloseBrackets = 0;

    // loop through each character in the expression
    for(int i = 0; i < size; i++) {
    	if (exp[i] == DERIVATIVE || exp[i] == LIMIT) {
            char mode = exp[i];
    		// Find the comma
    		int j, arg0Size = 0, arg1Size = 0;
    		for(j = i + 2; j < size; j++) {
    			if (exp[j] == BRACKET_OPEN)
					numOpenBrackets++;
    			else if (exp[j] == BRACKET_CLOSE)
					numCloseBrackets++;

    			if(exp[j] == COMMA && numOpenBrackets == numCloseBrackets){
    				arg0Size = j - i - 2;
    			} else if(exp[j+1] == BRACKET_CLOSE && numOpenBrackets == numCloseBrackets){
    				arg1Size = j - i - 2 - arg0Size;
    				break;
    			}
    		}

    		double x0 = ExpEvaluate(exp + arg0Size + 3, arg1Size, errorCode);

            double result;
            if (mode == DERIVATIVE)
			    result = derivative(exp + i + 2, x0, arg0Size, errorCode);
            else if (mode == LIMIT)
			    result = limit(exp + i + 2, x0, arg0Size, errorCode);

			i += arg0Size + arg1Size + 3;

            push(&operands, result, errorCode);
    	}
        // if the character is a digit or a decimal point or a negative sign,
        // read the whole number and push it into operands stack
    	else if (exp[i] <= DOT || (exp[i] == MINUS && exp[i+1] <= DOT &&
    			(i == 0 || exp[i-1] == BRACKET_OPEN || is_operator(exp[i-1]))
    	)) {

            int endi;
            double value = itod(&exp[i], exp + size, &endi);

            if(exp[endi + 1] == DOT && exp[endi + 2] > NINE) { // after DOT must be a number
				*errorCode = 1;
				return 0;
			}

			i += endi - 1;

            // push the value into operands stack
            push(&operands, value, errorCode);

            // check if not the last digit, append *
            if (i < size - 1  && (exp[i+1] > BRACKET_CLOSE || exp[i+1] == ANSWER) && exp[i+1] != COMMA)
                push(&operators, MULTIPLY, errorCode);
		} else if (exp[i] == PINFTY) {
            push(&operands, DBL_MAX, errorCode);
		} else if (exp[i] == NINFTY) {
            push(&operands, -DBL_MAX, errorCode);
		}  else if (exp[i] == MINUS && is_variable(exp[i+1]) &&
    			(i == 0 || exp[i-1] == BRACKET_OPEN || is_operator(exp[i-1]))) {
            i++;
    		push(&operands, -GetVar(exp[i]), errorCode);
            if (i < size - 1  && (exp[i+1] > BRACKET_CLOSE || exp[i+1] == ANSWER) && exp[i+1] != COMMA)
                push(&operators, MULTIPLY, errorCode);
		 } else if (is_variable(exp[i])) {
            push(&operands, GetVar(exp[i]), errorCode);
            if (i < size - 1  && (exp[i+1] > BRACKET_CLOSE || exp[i+1] == ANSWER) && exp[i+1] != COMMA)
                push(&operators, MULTIPLY, errorCode);
		}
        // if the character is an opening parenthesis,
        // push it into operators stack
        else if (exp[i] == BRACKET_OPEN) {
            push(&operators, exp[i], errorCode);
            numOpenBrackets++;
        }

        // if the character is a closing parenthesis,
        // pop and evaluate all operators until an opening parenthesis is found
        else if (exp[i] == BRACKET_CLOSE) {
            while (operators.top != -1 && operators.items[operators.top] != BRACKET_OPEN) {
            	calculate(&operators, &operands, errorCode);
            }

            // pop and discard the opening parenthesis
            if (operators.top != -1) {
                pop(&operators, errorCode);
            }
            else {
                *errorCode = 3;
                return 0;
            }
            numCloseBrackets++;
        }

        // if the character is a comma,
        else if (exp[i] == COMMA) {
			while (operators.top != -1 && operators.items[operators.top] != BRACKET_OPEN) {
            	calculate(&operators, &operands, errorCode);
			}
		}

        // if the character is an operator,
        // pop and evaluate all operators with higher or equal precedence
        else if (is_operator(exp[i])) {
        	// Shouldn't any number after FACTORIAL
            if (exp[i] == FACTORIAL && i < size - 1 && exp[i+1] <= DOT) {
                *errorCode = 1;
                return 0;
            }
            while (operators.top != -1 && precedence(operators.items[operators.top]) >= precedence(exp[i])) {
            	calculate(&operators, &operands, errorCode);
            }

            // push the current operator into operators stack
            push(&operators, exp[i], errorCode);
		}

        // if the character is invalid, print an error message and exit
        else {
            // Invalid character
        }
    }

    if(numCloseBrackets != numOpenBrackets) {
        *errorCode = 3;
        return 0;
    }

    // pop and evaluate all remaining operators
    while (operators.top != -1) {
    	calculate(&operators, &operands, errorCode);
    }

    // the final result is the only value left in operands stack
    if (operands.top > 0)
        *errorCode = 1;

    answer = pop(&operands, errorCode);
    if (*errorCode == 0)
    	SetVar(ANSWER, answer);
    return answer;
}

// FIND LIMIT
double limit(char *exp, double x0, char size, uint8_t *error) {
    double h = TOLERANCE; // small number for approximating limit
    double t = GetVar(X);
    SetVar(X, x0 + h);
    double left = ExpEvaluate(exp, size, error);
    SetVar(X, x0 - h);
    double right = ExpEvaluate(exp, size, error);

    SetVar(X, t);

    if (isnan(left) || isnan(right)) { // handle cases where limit is undefined
        return NAN;
    } else if ((isinf(left) && isinf(right)) || (fabs(left - right) < sqrt(TOLERANCE))) {
        return (left+right)/2;
    } else { // handle cases where limit exists but is different from both sides
        return NAN;
    }
}

// FIND DERIVATIVE
// Using central difference formula
double derivative(char *exp, double x, char size, uint8_t *error) {
    double h = 1e-4;  // initial step size
    double t = GetVar(X);

    double result, prev_result;
    for(int i = 0 ; i < 10; i++) {
        SetVar(X, x+h);
        double f1 = ExpEvaluate(exp, size, error);
        SetVar(X, x-h);
        double f2 = ExpEvaluate(exp, size, error);
        result = (f1 - f2)/(2*h);
        if(i == 0) {
        	 prev_result = result;
        	 continue;
        }
        double delta = fabs(prev_result - result);
        if(delta < TOLERANCE) {
            break;
        } else if (delta > 1) {
            result = prev_result;
            break;
        } else {
            h /= 10.0;
            prev_result = result;
        }
    }

    SetVar(X, t);
    return result;
}

// SOLVE
double ExpSolve(char *exp, char size, uint8_t *error) {
    double xo = GetVar(X);
    double x = GetVar(X);
    double t = GetVar(X);
    double fx, dfx;
    double h = 1;

    int iterations = 0;
    while (fabs(h) >= TOLERANCE  || isnan(h)) {
        iterations++;
        fx = ExpEvaluate(exp, size, error);
        dfx = derivative(exp, x, size, error);
        h = fx/dfx;
        if(isnan(h)) {
            xo += 10*xo + TOLERANCE;
            x = xo;
        } else
            x = x - h;
        SetVar(X, x);

        if(iterations == 500) {
            x = t - TOLERANCE;
            SetVar(X, t);
        } else if (iterations > 1000){
            x = NAN;
            break;
        }
    }
    return x;
}

double evaluate(char *exp, uint8_t size, uint8_t* errorCode) {
    // Find if there is an EQUAL_SIGN
    for(int i = 0; i < size; i++) {
        if(exp[i] == EQUAL_SIGN) {
            if(i == 0 || i == size - 1) {
                *errorCode = 1;
                return 0;
            }
            double result = 0.0;
            if(exp[i+1] == EQUAL_SIGN) {
                char _exp[size + 1];
                for (int j = 0; j < size; j++)
                    _exp[j] = exp[j];
                _exp[i] = MINUS;
                _exp[i+1] = BRACKET_OPEN;
                _exp[size] = BRACKET_CLOSE;
                result = ExpSolve(_exp, size + 1, errorCode);
                SetVar(ANSWER, result);
                return result;
            } else {
                if (i == 1 && is_variable(exp[i - 1])) {
                    result = ExpEvaluate(exp + 2, size - 2, errorCode);
                    if (*errorCode == 0)
                        SetVar(exp[i-1], result);
                } else if (i == size - 2 && is_variable(exp[i + 1])) {
                    result = ExpEvaluate(exp, size - 2, errorCode);
                    if (*errorCode == 0)
                        SetVar(exp[i+1], result);
                }
                return result;
            }
        }
    }
	return ExpEvaluate(exp, size, errorCode);
}

// CONVERT DOUBLE TO FRACTION
// Using continued fraction expansion algorithm
Fraction to_fraction(double x) {
    Fraction frac = {1, 0, 0}; // initialize to positive fraction
    if (x == 0) {
        frac.num = 0;
        frac.den = 1;
        return frac;
    } else if (x < 0) {
        frac.sign = -1; // set sign to negative
        x = -x; // use absolute value for fraction conversion
    }
    long long int a = 0, b = 1, c = 1, d = 0;
    double eps = 10e-12;
    while (1) {
        long long int p = a + c;
        long long int q = b + d;
        double r = p * 1.0 / q;
        if (r > x + eps) {
            c = p;
            d = q;
        } else if (r < x - eps) {
            a = p;
            b = q;
        } else {
            frac.num = p;
            frac.den = q;
            break;
        }
    }
    return frac;
}
