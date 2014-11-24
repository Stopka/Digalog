#pragma once
#define FONT_HOUR 0
#define FONT_MINUTE 0
#define FONT_DATE 1
#define FONT_DAY 1
	
void fonts_init();
void fonts_deinit();

GFont fonts_get(uint8_t i);