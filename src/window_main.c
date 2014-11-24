#include <pebble.h>
#include "window_main.h"

#define TEXT_LAYERS_COUNT 4
#define TEXT_LAYER_DAY 0
#define TEXT_LAYER_DATE 1
#define TEXT_LAYER_HOUR 2
#define TEXT_LAYER_MINUTE 3
TextLayer* text_layers[TEXT_LAYERS_COUNT];
char* text_layer_buffers[TEXT_LAYERS_COUNT];

#define BITMAP_LAYERS_COUNT 3
#define BITMAP_LAYER_BACKGROUND 0
#define BITMAP_LAYER_NOTIFICATION 1 //2 items
BitmapLayer* bitmap_layer[TEXT_LAYERS_COUNT];
InverterLayer* inverter_layer;

int battery_state=0;
bool bluetooth_state=false;

static Window*    window;



static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);


void window_main_show() {
	window_stack_push(window, true);
}

void window_main_init(){
	window = window_create();
	//window_set_background_color	(window,GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_main_deinit(){
	window_destroy(window);
	window=NULL;
}

void handle_tick(struct tm* tick_time, TimeUnits units_changed){
	//Hours
	strftime((char*)text_layer_buffers[TEXT_LAYER_HOUR],3,clock_is_24h_style()?"%H":"%I",tick_time);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_HOUR]);
	//Minutes
	strftime((char*)text_layer_buffers[TEXT_LAYER_MINUTE],3,"%M",tick_time);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_MINUTE]);
	//Week day
	strftime((char*)text_layer_buffers[TEXT_LAYER_DAY],30,"%A",tick_time);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_DAY]);
	//Date
	strftime((char*)text_layer_buffers[TEXT_LAYER_DATE],30,"%d. %m. %Y",tick_time);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_DATE]);
	//Analog
	//layer_mark_dirty(analog_time[0]);
	//layer_mark_dirty(analog_time[1]);
}

static void window_load(Window* window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	text_layer_buffers[TEXT_LAYER_DAY]=(char *)malloc(31*sizeof(char));
	text_layers[TEXT_LAYER_DAY] = text_layer_create(GRect(0, -5, bounds.size.w, 28));
  text_layer_set_text(text_layers[TEXT_LAYER_DAY], text_layer_buffers[TEXT_LAYER_DAY]);
	text_layer_set_font(text_layers[TEXT_LAYER_DAY],fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_DAY],GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_DAY]));
	
	text_layer_buffers[TEXT_LAYER_DATE]=(char *)malloc(31*sizeof(char));
	text_layers[TEXT_LAYER_DATE] = text_layer_create(GRect(0, bounds.size.h-28, bounds.size.w, 24));
  text_layer_set_text(text_layers[TEXT_LAYER_DATE], text_layer_buffers[TEXT_LAYER_DATE]);
	text_layer_set_font(text_layers[TEXT_LAYER_DATE],fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_DATE],GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_DATE]));
	
	text_layer_buffers[TEXT_LAYER_HOUR]=(char *)malloc(3*sizeof(char));
	text_layers[TEXT_LAYER_HOUR] = text_layer_create(GRect(0, bounds.size.h/2-(34/2), bounds.size.w/2, 34));
  text_layer_set_text(text_layers[TEXT_LAYER_HOUR], text_layer_buffers[TEXT_LAYER_HOUR]);
	text_layer_set_font(text_layers[TEXT_LAYER_HOUR],fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_HOUR],GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_HOUR]));
	
	text_layer_buffers[TEXT_LAYER_MINUTE]=(char *)malloc(3*sizeof(char));
	text_layers[TEXT_LAYER_MINUTE] = text_layer_create(GRect(bounds.size.w/2, bounds.size.h/2-(34/2), bounds.size.w/2, 34));
  text_layer_set_text(text_layers[TEXT_LAYER_MINUTE], text_layer_buffers[TEXT_LAYER_MINUTE]);
	text_layer_set_font(text_layers[TEXT_LAYER_MINUTE],fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_MINUTE],GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_MINUTE]));
	
	inverter_layer=inverter_layer_create(GRect(0,0, bounds.size.w, bounds.size.h));
	layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
}

static void window_appear(Window *window) {
	time_t now_time_t=time(NULL);	
	struct tm* now_tm=localtime(&now_time_t);
	handle_tick(now_tm,MINUTE_UNIT);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void window_disappear(Window *window) {
	tick_timer_service_unsubscribe();
}


static void window_unload(Window* window){
	for(uint8_t i=0;i<TEXT_LAYERS_COUNT;i++){
		if(text_layers[i]!=NULL){
			text_layer_destroy(text_layers[i]);
			text_layers[i]=NULL;
			free(text_layer_buffers[i]);
		}
	}
	inverter_layer_destroy(inverter_layer);
}