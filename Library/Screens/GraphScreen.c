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

void GraphScreen(char *exp, uint8_t size, uint8_t* errorCode) {
    double t = GetVar(X);
	Graph(input, input_length, errorCode);

	if (*errorCode > 0) {
	    SetVar(X, t);
		return;
	}

    ST7920_Update();
    while (1) {
		uint8_t key = KeyPad_WaitForKeyGetChar(0, true);
		if (key != 0xFF) {
		    double step = (scale * 2 / 128) * 5;

			switch (key) {
			case LEFT:
				centerX += step;
				break;

			case RIGHT:
				centerX -= step;
				break;

			case DOWN:
				centerY += step;
				break;

			case UP:
				centerY -= step;
				break;

			case PLUS:
				scale -= 0.5;
				break;

			case MINUS:
				scale += 0.5;
				break;

			case TEST:
				scale = 1;
				centerX = 1; centerY = 0.5;
				break;

			default:
			    SetVar(X, t);
				return;
			}

			Graph(input, input_length, errorCode);
		    ST7920_Update();
		}
	}
}
