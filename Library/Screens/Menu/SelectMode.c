uint8_t maxRow = 1;

uint8_t SelectModeMenu() {
	uint8_t selectedRow = currentMode;
	GLCD_Buf_Clear();
	GLCD_Font_Print(0, 0, "Math Mode");
	GLCD_Font_Print(0, 1, "Graphing Mode");

	ToggleRow(selectedRow);
	ST7920_Update();

	while (1) {
		uint8_t key = KeyPad_WaitForKeyGetChar(0, false);
		if (key != 0xFF) {
			if(key == DOWN && selectedRow < maxRow) {
				ToggleRow(selectedRow);
				selectedRow++;
				ToggleRow(selectedRow);
			} else if (key == UP && selectedRow > 0) {
				ToggleRow(selectedRow);
				selectedRow--;
				ToggleRow(selectedRow);
			} else if (key == EQUAL) {
				return selectedRow;
			} else if (key == AC) {
				return currentMode;
			}
			else continue;

			ST7920_Update();
		}
	}
}
