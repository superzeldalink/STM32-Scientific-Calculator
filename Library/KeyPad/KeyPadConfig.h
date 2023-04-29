#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H

#define           _KEYPAD_DEBOUNCE_TIME_US        2
#define           _KEYPAD_USE_FREERTOS            0

const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] =
{
		COL0_GPIO_Port,
		COL1_GPIO_Port,
		COL2_GPIO_Port,
		COL3_GPIO_Port,
		COL4_GPIO_Port
};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
		COL0_Pin,
		COL1_Pin,
		COL2_Pin,
		COL3_Pin,
		COL4_Pin
};

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] =
{
		ROW0_GPIO_Port,
		ROW1_GPIO_Port,
		ROW2_GPIO_Port,
		ROW3_GPIO_Port,
		ROW4_GPIO_Port,
		ROW5_GPIO_Port,
		ROW6_GPIO_Port,
		ROW7_GPIO_Port
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
		ROW0_Pin,
		ROW1_Pin,
		ROW2_Pin,
		ROW3_Pin,
		ROW4_Pin,
		ROW5_Pin,
		ROW6_Pin,
		ROW7_Pin,
};

#endif
