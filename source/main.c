#include "common.h"
#include "draw.h"
#include "hid.h"
#include "i2c.h"

#include "font_pbm.h"

#define DISPLAY_TEXT "Hello FIRM Entrypoint!"
#define START_X 10
#define START_Y 10

void Reboot()
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
	while(true);
}


void PowerOff()
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);
	while (true);
}

void Setup()
{
	ClearScreenFull(true, true);
	SetFontFromPbm(font_pbm, font_pbm_size);
}

int main()
{
	char clear_text[64] = { 0 };
	bool use_top = true;
	u32 cur_x = START_X;
	u32 cur_y = START_Y;
	
	Setup();
	
	while( true ) {
		DrawStringF(cur_x, cur_y, use_top, DISPLAY_TEXT);
		u32 pad_state = InputWait();
		u32 increment = 5;
		DrawStringF(cur_x, cur_y, use_top, clear_text);
		if (pad_state & (BUTTON_B | BUTTON_START))
			break;
		if (pad_state & BUTTON_L1)
			increment = 10;
		if (pad_state & BUTTON_R1) {
			DrawString((use_top) ? TOP_SCREEN : BOT_SCREEN, DISPLAY_TEXT, cur_x, cur_y, STD_COLOR_BG, STD_COLOR_BG);
			cur_x = START_X;
			cur_y = START_Y;
			use_top = !use_top;
		}
		if(pad_state & BUTTON_ARROW)
			DrawString((use_top) ? TOP_SCREEN : BOT_SCREEN, DISPLAY_TEXT, cur_x, cur_y, STD_COLOR_BG, STD_COLOR_BG);
		if (pad_state & BUTTON_UP)
			cur_y = (cur_y >= increment) ? cur_y - increment : 0;
		else if (pad_state & BUTTON_LEFT)
			cur_x = (cur_x >= increment) ? cur_x - increment : 0;
		else if (pad_state & BUTTON_DOWN)
			cur_y += increment;
		else if (pad_state & BUTTON_RIGHT)
			cur_x += increment;
	}
	
	Reboot();
	return 0;
}
