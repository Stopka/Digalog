#include <pebble.h>
#include "builder.h"

Layer* builder_layer(GRect bounds,Layer* window_layer,LayerUpdateProc renderer){
	Layer* layer=layer_create(bounds);
	layer_set_clips	(layer,false);
	layer_set_update_proc(layer, renderer);
	layer_add_child(window_layer,layer);
	return layer;
}

TextLayer* builder_text_layer(GRect bounds,Layer* window_layer,uint8_t char_count,GTextAlignment align,uint8_t font){
	char * text_buffer=(char *)malloc(char_count*sizeof(char));
	TextLayer* layer = text_layer_create(bounds);
  text_layer_set_text(layer, text_buffer);
	text_layer_set_font(layer,fonts_get(font));
	text_layer_set_text_alignment(layer,align);
	text_layer_set_background_color(layer,GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(layer));
	return layer;
}