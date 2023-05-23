double scale = 2;
double centerX = 2, centerY = 1;
double cursorX = 64;
bool cursorMode = false;

uint8_t orig_graph[1024];

void Graph(uint8_t *exp, uint8_t size, uint8_t* errorCode) {
	GLCD_Buf_Clear();
    double step = scale * 2 / 128;

	int dispX0 = centerX / step;
	int dispY0 = -(centerY / step) + 64;

	if(dispX0 >= 0 && dispX0 < 128)
		DrawLine(dispX0, 0, dispX0, 63);

	if(dispY0 >= 0 && dispY0 < 64)
		DrawLine(0, dispY0, 127, dispY0);

    for(int i = 0; i < 127; i++) {
		SetVar(X, i*step - centerX);
		double y1 = evaluate(exp, size, errorCode);
		if(*errorCode == 4) {
			y1 = NAN;
			*errorCode = 0;
		}

		SetVar(X, (i+1)*step - centerX);
		double y2 = evaluate(exp, size, errorCode);

		if(*errorCode == 4) {
			y2 = NAN;
			*errorCode = 0;
		}
		if(*errorCode > 0)
			return;

		int dispY1 = (y1 + centerY) / step;
		int dispY2 = (y2 + centerY) / step;

		dispY1 = -dispY1 + 64;
		dispY2 = -dispY2 + 64;

		if(!((dispY1 < 0 || dispY1 > 63) && (dispY2 < 0 || dispY2 > 63))) {
			if(dispY1 < 0) dispY1 = 0;
			if(dispY1 > 64) dispY1 = 63;
			if(dispY2 < 0) dispY2 = 0;
			if(dispY2 > 64) dispY2 = 63;
			DrawLine(i, dispY1,  i + 1, dispY2);
		}
	}
}

void GraphScreen(uint8_t *exp, uint8_t size, uint8_t* errorCode) {
    double t = GetVar(X);
	Graph(exp, input_length, errorCode);

	if (*errorCode > 0) {
	    SetVar(X, t);
		return;
	}

    ST7920_Update();
    while (1) {
		uint8_t key = KeyPad_WaitForKeyGetChar(KEY_TIMEOUT_MS, true);
		if (key != 0xFF) {
		    double step = (scale * 2 / 128);
		    double moveStep = step * 5;

			if(!cursorMode) {
				switch (key) {
					case LEFT:
						centerX += moveStep;
						break;

					case RIGHT:
						centerX -= moveStep;
						break;

					case DOWN:
						centerY += moveStep;
						break;

					case UP:
						centerY -= moveStep;
						break;

					case PLUS:
						scale -= 0.5;
						break;

					case MINUS:
						scale += 0.5;
						break;

					case ZERO:
						scale = 1;
						centerX = 1; centerY = 0.5;
						break;

					case TEST:
						memcpy(orig_graph, GLCD_Buf, 1024);
						cursorX = 64;
						cursorMode = true;
						break;

					default:
						SetVar(X, t);
						return;
				}
			} else {
				switch (key) {
					case LEFT:
						if(cursorX > 0)
							cursorX -= 1;
						break;

					case RIGHT:
						if(cursorX < 127)
							cursorX += 1;
						break;

					default:
						cursorMode = false;
						memcpy(GLCD_Buf, orig_graph, 1024);
						ST7920_Update();
						continue;
				}
			}

			if(cursorMode) {
				memcpy(GLCD_Buf, orig_graph, 1024);
				double x = cursorX*step - centerX;
				SetVar(X, x);
				double y = evaluate(exp, size, errorCode);
				if(*errorCode == 4) {
					y = NAN;
					*errorCode = 0;
				}

				uint8_t cursorY = (y + centerY) / step;
				cursorY = -cursorY + 64;

				DrawDashedLine(cursorX, 0, cursorX, 63, 2, 3);
				if(cursorY < 64 && cursorY >= 0)
					DrawDashedLine(0, cursorY, 127, cursorY, 2, 3);

				sprintf((char*)answerRow_buf, "%0.3g;%0.3g", x, y);
				PrintAnswer();
			} else
				Graph(exp, input_length, errorCode);
		    ST7920_Update();
		}
	}
}
