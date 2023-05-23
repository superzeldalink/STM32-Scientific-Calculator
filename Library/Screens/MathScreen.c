char disp_buff[256];
uint8_t charLength[256];

bool cursorState = false;
uint8_t cursor_ptr = 0;

void ToggleCursor() {
	cursorState = !cursorState;
	ToggleRectangle(disX * 8, disY * 8, 1, 6);
}
void ToggleCursorOnOff(bool isOn) {
	cursorState = isOn;
	if(isOn)
		ToggleRectangle(disX * 8, disY * 8, 1, 6);
}

char* GetChar(uint8_t ch) {
	if (ch < 10) {
		static char c[] = { 0, 0 };
		c[0] = ch + 48;
		return c;
	} else if (ch == LN) {
		static char c[] = { 'l', 'n', 0 };
		return c;
	} else if (ch == DERIVATIVE) {
		static char c[] = { 'd', '(', 0 };
		return c;
	} else if (ch == LIMIT) {
		static char c[] = { 'l', '(', 0 };
		return c;
	} else if (ch == INTEGRAL) {
		static char c[] = { 'i', '(', 0 };
		return c;
	} else if (ch == SINE || ch == COSINE || ch == TANGENT || ch == ANSWER
			|| ch == LOG || ch == ABS) {
		static char c[] = { 0, 0, 0, 0 };
		switch (ch) {
		case SINE:
			c[0] = 's';
			c[1] = 'i';
			c[2] = 'n';
			break;
		case COSINE:
			c[0] = 'c';
			c[1] = 'o';
			c[2] = 's';
			break;
		case TANGENT:
			c[0] = 't';
			c[1] = 'a';
			c[2] = 'n';
			break;
		case ANSWER:
			c[0] = 'A';
			c[1] = 'n';
			c[2] = 's';
			break;
		case LOG:
			c[0] = 'l';
			c[1] = 'o';
			c[2] = 'g';
			break;
		case ABS:
			c[0] = 'a';
			c[1] = 'b';
			c[2] = 's';
			break;
		}
		return c;
	} else if (ch == LOGX) {
		static char c[] = { 'l', 'o', 'g', '(', 0 };
		return c;
	} else {
		static char c[] = { 0, 0 };
		switch (ch) {
		case DOT:
			c[0] = '.';
			break;
		case COMMA:
			c[0] = ',';
			break;
		case EQUAL:
			c[0] = '=';
			break;
		case PLUS:
			c[0] = '+';
			break;
		case MINUS:
			c[0] = '-';
			break;
		case MULTIPLY:
			c[0] = 'x';
			break;
		case DIVIDE:
			c[0] = '/';
			break;
		case BRACKET_OPEN:
			c[0] = '(';
			break;
		case BRACKET_CLOSE:
			c[0] = ')';
			break;
		case EXPONENT:
			c[0] = '^';
			break;
		case FACTORIAL:
			c[0] = '!';
			break;
		case SQRT:
			c[0] = 128;
			break;
		case XRT:
			c[0] = 129;
			break;
		case X:
			c[0] = 130;
			break;
		case Y:
			c[0] = 131;
			break;
		case Z:
			c[0] = 132;
			break;
		case EQUAL_SIGN:
			c[0] = '=';
			break;
		case PINFTY:
			c[0] = PINFTY;
			break;
		case NINFTY:
			c[0] = NINFTY;
			break;
		}
		return c;
	}
}

void UpdateCursor() {
	disX = 0;
	for (int i = 0; i < input_ptr; i++) {
		disX += charLength[i];
	}

	disY = disX / 16;
	disX = disX % 16;
}

void UpdateDisp() {
	disp_buff[0] = '\0';
	for (int i = 0; i < input_length; i++) {
		if (input[i] == BRACKET_OPEN
				&& (input[i - 1] == LOGX || input[i - 1] == DERIVATIVE
						|| input[i - 1] == LIMIT || input[i - 1] == INTEGRAL)) {
			charLength[i] = 0;
		} else {
			char *c = GetChar(input[i]);
			strcat(disp_buff, c);
			charLength[i] = strlen(c);
		}
	}
	strcat(disp_buff, "     ");

	GLCD_Font_Print(0, 0, disp_buff);
	ST7920_Update();
	UpdateCursor();
}

void AddKey(uint8_t key) {
	if (key == LOGX || key == DERIVATIVE || key == LIMIT || key == INTEGRAL) {
		input_length += 2;
		for (int i = input_length - 1; i >= input_ptr + 2; i--) {
			input[i] = input[i - 2];
		}
		input[input_ptr++] = key;

		input[input_ptr++] = BRACKET_OPEN;
	} else {
		input_length++;
		for (int i = input_length - 1; i >= input_ptr + 1; i--) {
			input[i] = input[i - 1];
		}
		input[input_ptr++] = key;
	}
}

void GoLeft() {
	uint8_t prevKey = input[input_ptr - 1];
	uint8_t preprevKey = input[input_ptr - 2];
	input_ptr--;

	if (prevKey == BRACKET_OPEN
			&& (preprevKey == LOGX || preprevKey == LIMIT
					|| preprevKey == DERIVATIVE || preprevKey == INTEGRAL)) {
		input_ptr--;
	}
}

void GoRight() {
	uint8_t nextKey = input[input_ptr + 1];
	uint8_t nextNextKey = input[input_ptr + 2];
	input_ptr++;

	if (nextNextKey == BRACKET_OPEN
			&& (nextKey == LOGX || nextKey == LIMIT || nextKey == DERIVATIVE || nextKey == INTEGRAL)) {
		input_ptr++;
	}
}

void BackSpace() {
	uint8_t prevKey = input[input_ptr - 1];
	uint8_t preprevKey = input[input_ptr - 2];

	if (prevKey == BRACKET_OPEN
			&& (preprevKey == LOGX || preprevKey == LIMIT
					|| preprevKey == DERIVATIVE || preprevKey == INTEGRAL)) {
		input_ptr -= 2;
		for (int i = input_ptr; i < input_length - 1; i++) {
			input[i] = input[i + 2];
		}
		input_length -= 2;
	} else {
		input_ptr--;
		for (int i = input_ptr; i < input_length - 1; i++) {
			input[i] = input[i + 1];
		}
		input_length--;
	}
}

void PrintError(uint8_t errorCode) {
	ClearRow(ANSWER_ROW);
	char errorText[5];
	sprintf(errorText, "%d", errorCode);
	switch (errorCode) {
	case 0:
		break;
	case 1:
	case 2:
		GLCD_Font_Print(4, ANSWER_ROW, "Syntax error");
		break;
	case 3:
		GLCD_Font_Print(1, ANSWER_ROW, "Missing bracket");
		break;
	case 4:
		GLCD_Font_Print(7, ANSWER_ROW, "Div. by 0");
		break;
	case 33:
		GLCD_Font_Print(4, ANSWER_ROW, "Out of range");
		break;
	default:
		GLCD_Font_Print(6, ANSWER_ROW, "Math error");
//		GLCD_Font_Print(6, ANSWER_ROW, errorText);
		break;
	}
}

//void ShowReceiving() {
//	loadingShown = true;
//	memcpy(prev_GLCD_Buf, GLCD_Buf, 1024);
//	GLCD_Buf_Clear();
//	GLCD_Font_Print(0, 0, "Receiving...");
//	ST7920_Update();
//}

void ShowAnswer(double answer, uint8_t errorCode) {
	if(loadingShown) {
		memcpy(GLCD_Buf, prev_GLCD_Buf, 1024);
		loadingShown = false;
	}
	if (errorCode == 0) {
		if(answer == DBL_MAX) {
			answerRow_buf[0] = PINFTY;
			answerRow_buf[1] = 0;
		} else if(answer == -DBL_MAX) {
			answerRow_buf[0] = NINFTY;
			answerRow_buf[1] = 0;
		} else if (answer == 1 / DBL_MAX || answer == 1 / (-DBL_MAX)) {
			answerRow_buf[0] = '0';
			answerRow_buf[1] = 0;
		} else {
			sprintf(answerRow_buf, "%0.10g", answer);
		}
		PrintAnswer();
	} else {
		PrintError(errorCode);
	}
}

void MathScreen() {
	UpdateDisp();
	HAL_TIM_Base_Start_IT(&htim2);
	while (1) {
		uint8_t key = KeyPad_WaitForKeyGetChar(KEY_TIMEOUT_MS, false);
		if (key != 0xFF) {
			HAL_TIM_Base_Stop_IT(&htim2);
			if (cursorState)
				ToggleCursor();

			switch (key) {
			case BACKSPACE:
				if (input_ptr > 0) {
					BackSpace();
				}
				break;

			case EQUAL: {
				if(input_length > 0) {
					if(currentMode == 0) {
						uint8_t errorCode = 0;
						double answer = 0;

						if(uartMode) {
							uartDone = false;
							HAL_UART_Transmit(&huart1, input, input_length, 100);

							millis = HAL_GetTick();

							while(HAL_GetTick() - millis <= 3000) {
								if(uartDone) break;
							}

							if(!uartDone) {
								strcpy(answerRow_buf, "Receive Failed.");
								PrintAnswer();
							} else {
								memcpy(&answer, RxBuf, sizeof(double));
								errorCode = RxBuf[8];

								if(errorCode == 0) SetVar(ANSWER, answer);
								ShowAnswer(answer, errorCode);
							}

						} else {
							answer = evaluate(input, input_length, &errorCode);
							ShowAnswer(answer, errorCode);
						}

						input_ptr = input_length;

					} else if (currentMode == 1) {
						uint8_t errorCode = 0;
						GraphScreen(input, input_length, &errorCode);

						if(loadingShown) {
							memcpy(GLCD_Buf, prev_GLCD_Buf, 1024);
							loadingShown = false;
						}

						if (errorCode > 0) {
							PrintError(errorCode);
						}
					}
				}
				break;
			}

			case AC:
				input_ptr = 0;
				input_length = 0;
				disX = 0;
				disY = 0;
				ST7920_Clear();
				break;

			case LEFT:
				if (input_ptr > 0) {
					GoLeft();
				}
				break;

			case RIGHT:
				if (input_ptr < input_length) {
					GoRight();
				}
				break;

			case DOWN: {
				uint8_t prevKey = input[input_ptr - 1];
				uint8_t prevPrevKey = input[input_ptr - 2];
				BackSpace();

				switch (prevKey) {
				case EXPONENT:
					AddKey(SQRT);
					break;
				case SQRT:
					AddKey(XRT);
					break;
				case XRT:
					AddKey(EXPONENT);
					break;

				case SINE:
					AddKey(COSINE);
					break;
				case COSINE:
					AddKey(TANGENT);
					break;
				case TANGENT:
					AddKey(SINE);
					break;

				case LN:
					AddKey(LOG);
					break;
				case LOG:
					AddKey(LOGX);
					break;

				case DOT:
					AddKey(COMMA);
					break;
				case COMMA:
					AddKey(DOT);
					break;

				case X:
					AddKey(Y);
					break;
				case Y:
					AddKey(Z);
					break;
				case Z:
					AddKey(X);
					break;

				case ZERO:
					AddKey(PINFTY);
					break;
				case PINFTY:
					AddKey(NINFTY);
					break;
				case NINFTY:
					AddKey(ZERO);
					break;

				case BRACKET_OPEN:
					switch (prevPrevKey) {
					case LIMIT:
						AddKey(DERIVATIVE);
						break;
					case DERIVATIVE:
						AddKey(INTEGRAL);
						break;
					case INTEGRAL:
						AddKey(LIMIT);
						break;
					case LOGX:
						AddKey(LN);
						break;
					default:
						AddKey(prevKey);
						break;
					}
					break;
				default:
					AddKey(prevKey);
					break;
				}
				break;
			}

			case S2D: {
				if(currentMode == 0) {
					Fraction frac = to_fraction(answer);
					sprintf(answerRow_buf, "%ld/%ld", frac.num * frac.sign, frac.den);
					PrintAnswer();
				}
				break;
			}

			case MODE: {
				if(ChangeScreen()) return;
				break;
			}

			case TEST: {
				if(currentMode == 0) {
					uartMode = !uartMode;
					TogglePixel(127, 0);
				}
				break;
			}

			default:
				AddKey(key);
				break;
			}

			UpdateDisp();
			HAL_TIM_Base_Start_IT(&htim2);
		}
	}
}
