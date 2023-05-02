double scale = 1;
double centerX = 1, centerY = 0.5;

void Graph(char *exp, uint8_t size, uint8_t* errorCode) {
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

		SetVar(X, (i+1)*step - centerX);
		double y2 = evaluate(exp, size, errorCode);

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

void GraphScreen(char *exp, uint8_t size, uint8_t* errorCode) {
    double t = GetVar(X);
	Graph(input, input_length, errorCode);

	if (*errorCode > 0) {
	    SetVar(X, t);
		return;
	}

    ST7920_Update();
    while (1) {
		uint8_t key = KeyPad_WaitForKeyGetChar(0);
		if (key != 0xFF) {
		    double step = (scale * 2 / 128) * 5;

			switch (key) {
			case LEFT:
				centerX += step;
				Graph(input, input_length, errorCode);
				break;

			case RIGHT:
				centerX -= step;
				Graph(input, input_length, errorCode);
				break;

			case UP:
				centerY += step;
				Graph(input, input_length, errorCode);
				break;

			case DOWN:
				centerY -= step;
				Graph(input, input_length, errorCode);
				break;

			case PLUS:
				scale -= 0.5;
				Graph(input, input_length, errorCode);
				break;

			case MINUS:
				scale += 0.5;
				Graph(input, input_length, errorCode);
				break;

			case TEST:
				scale = 1;
				centerX = 1; centerY = 0.5;
				Graph(input, input_length, errorCode);
				break;

			default:
			    SetVar(X, t);
				return;
			}

		    ST7920_Update();
		}
	}
}
