#ifndef	_KEYPAD_H
#define	_KEYPAD_H

#include <stdint.h>
#include <stdbool.h>


/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    1.0.0
  
  
  Reversion History:
  
  (1.0.0)
  First Release.
*/

typedef struct
{
	uint8_t   ColumnSize;
	uint8_t   RowSize;
	uint16_t  LastKey;	
	
}KeyPad_t;

typedef enum
{
  ZERO = 0,
  ONE = 1,
  TWO = 2,
  THREE = 3,
  FOUR = 4,
  FIVE = 5,
  SIX = 6,
  SEVEN = 7,
  EIGHT = 8,
  NINE = 9,
  DOT = 10,
  EQUAL = 11,
  ANSWER = 12,
  PLUS = 13,
  MINUS = 14,
  MULTIPLY = 15,
  DIVIDE = 16,
  EXPONENT = 17,
  XRT = 18,
  FACTORIAL = 19,
  BACKSPACE = 20,
  BRACKET_CLOSE = 21,
  BRACKET_OPEN = 22,
  SQRT = 23,
  SINE = 24,
  COSINE = 25,
  TANGENT = 26,
  COTANGENT = 27,		// Not implemented
  LN = 28,
  LOG = 29,
  LOGX = 30,

  COMMA = 48,
  S2D = 49,
  UP = 50,
  DOWN = 51,
  LEFT = 52,
  RIGHT = 53,
  AC = 54,
  EQUAL_SIGN = 55,
  DERIVATIVE = 56,

  X = 97,
  Y = 98,
  Z = 99
} Key;

void      KeyPad_Init(void);
uint16_t  KeyPad_WaitForKey(uint32_t	Timeout_ms);
uint8_t      KeyPad_WaitForKeyGetChar(uint32_t	Timeout_ms);

#endif
