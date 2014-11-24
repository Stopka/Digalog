#include <pebble.h>
#include "fonts.h"
GFont font;

void fonts_init(){
	font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BOLD_38));
}

GFont fonts_get(uint8_t i){
	switch(i){
		case FONT_DATE:
			return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
		case FONT_DAY:
			return fonts_get_system_font(FONT_KEY_GOTHIC_24);
	}
	return font; //FONT_HOUR FONT_MINUTE
}

void fonts_deinit(){
	fonts_unload_custom_font(font);
}