#include "KeyPad.h"
#include "KeyPadConfig.h"
#include "stm32f4xx_ll_gpio.h"
#if (_KEYPAD_USE_FREERTOS==1)
#include "cmsis_os.h"
#define _KEYPAD_DELAY(x)      osDelay(x)
#else
#define _KEYPAD_DELAY(x)      HAL_Delay(x)
#endif

KeyPad_t	KeyPad;

//#############################################################################################
void	KeyPad_Init(void)
{
  GPIO_InitTypeDef	gpio;
  KeyPad.ColumnSize = sizeof(_KEYPAD_COLUMN_GPIO_PIN) / 2;
  KeyPad.RowSize = sizeof(_KEYPAD_ROW_GPIO_PIN) / 2;
  for(uint8_t	i=0 ; i<KeyPad.ColumnSize ; i++)
  {
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = _KEYPAD_COLUMN_GPIO_PIN[i];
    HAL_GPIO_Init((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[i], &gpio);
    HAL_GPIO_WritePin((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[i], _KEYPAD_COLUMN_GPIO_PIN[i], GPIO_PIN_SET);
  }
  for(uint8_t	i=0 ; i<KeyPad.RowSize ; i++)
  {
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = _KEYPAD_ROW_GPIO_PIN[i];
    HAL_GPIO_Init((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[i], &gpio);
  }
}
//#############################################################################################
uint16_t	KeyPad_Scan(void)
{
  uint16_t  key = 0;
  for(uint8_t c=0 ; c<KeyPad.ColumnSize ; c++)
  {
    for(uint8_t i=0 ; i<KeyPad.ColumnSize ; i++)
      HAL_GPIO_WritePin((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[i], _KEYPAD_COLUMN_GPIO_PIN[i], GPIO_PIN_SET);
    HAL_GPIO_WritePin((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[c], _KEYPAD_COLUMN_GPIO_PIN[c], GPIO_PIN_RESET);
    _KEYPAD_DELAY(5);
    for(uint8_t r=0 ; r<KeyPad.RowSize ; r++)
    {
      if(HAL_GPIO_ReadPin((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[r], _KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
      {
        _KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);
        if(HAL_GPIO_ReadPin((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[r], _KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
        {
          key |= r;
          key = (key << 8) | c;
          while(HAL_GPIO_ReadPin((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[r], _KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
            _KEYPAD_DELAY(5);
          return key;
        }
      }
    }
  }
  return 0xFFFF;
}
//#############################################################################################
uint16_t	KeyPad_WaitForKey(uint32_t  Timeout_ms)
{	
  uint16_t  keyRead;
  while(Timeout_ms==0)
  {
    keyRead = KeyPad_Scan();
		if(keyRead != 0xFFFF)
		{
			KeyPad.LastKey = keyRead;
			return keyRead;	
		}
		_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);	
	}
	uint32_t	StartTime = HAL_GetTick();
	while(HAL_GetTick()-StartTime < Timeout_ms)
	{
		keyRead = KeyPad_Scan();
		if(keyRead != 0xFFFF)
		{
			KeyPad.LastKey = keyRead;
			return keyRead;	
		}
		_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);	
	}
	KeyPad.LastKey = 0xFFFF;
	return 0xFFFF;
}
//#############################################################################################
uint8_t KeyPad_WaitForKeyGetChar(uint32_t Timeout_ms)
{
  switch(KeyPad_WaitForKey(Timeout_ms))
  {
	case 0x0000:
		return ZERO;
	case 0x0001:
		return DOT;
	case 0x0002:
		return EXPONENT;
	case 0x0003:
		return ANSWER;
	case 0x0004:
		return EQUAL;

	case 0x0100:
		return ONE;
	case 0x0101:
		return TWO;
	case 0x0102:
		return THREE;
	case 0x0103:
		return PLUS;
	case 0x0104:
		return MINUS;

	case 0x0200:
		return FOUR;
	case 0x0201:
		return FIVE;
	case 0x0202:
		return SIX;
	case 0x0203:
		return MULTIPLY;
	case 0x0204:
		return DIVIDE;

	case 0x0300:
		return SEVEN;
	case 0x0301:
		return EIGHT;
	case 0x0302:
		return NINE;
	case 0x0303:
		return BACKSPACE;
	case 0x0304:
		return AC;

	case 0x0400:
		return FACTORIAL;
	case 0x0401:
		return BRACKET_OPEN;
	case 0x0402:
		return BRACKET_CLOSE;
	case 0x0403:
		return SINE;
	case 0x0404:
		return S2D;

	case 0x0500:
		return EQUAL_SIGN;
	case 0x0501:
		return LEFT;
	case 0x0502:
		return DOWN;
	case 0x0503:
		return RIGHT;
	case 0x0504:
		return LN;

	case 0x0602:
		return UP;
	case 0x0603:
		return LIMIT;
	case 0x0604:
		return X;

	default:
		return 0xFF;
	}	
}
