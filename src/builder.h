#pragma once
#include "fonts.h"
	
Layer* builder_layer(GRect bounds,Layer* window_layer,LayerUpdateProc renderer);
TextLayer* builder_text_layer(GRect bounds,Layer* window_layer,uint8_t char_count,GTextAlignment align,uint8_t font);