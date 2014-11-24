#include <pebble.h>
#include "fonts.h"

#define FONTS_COUNT 2
GFont font[FONTS_COUNT];

void fonts_init(){
	font[FONT_DATE]=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_22));
	font[FONT_MINUTE]=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BOLD_38));
}

GFont fonts_get(uint8_t i){
	return font[i];
}

void fonts_deinit(){
	for(uint8_t i=0;i<FONTS_COUNT;i++){
		fonts_unload_custom_font(font[i]);
	}
}