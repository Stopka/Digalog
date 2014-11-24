#include <pebble.h>
#include "window_main.h"
#include "bitmap-loader.h"
#include "renderers.h"
#include "builder.h"

#define TEXT_LAYERS_COUNT 4
#define TEXT_LAYER_DAY 0
#define TEXT_LAYER_DATE 1
#define TEXT_LAYER_HOUR 2
#define TEXT_LAYER_MINUTE 3
TextLayer* text_layers[TEXT_LAYERS_COUNT];

BitmapLayer* bitmap_layer;

InverterLayer* inverter_layer;

#define LAYERS_COUNT 4
#define LAYER_CENTER 0
#define LAYER_HOUR 1
#define LAYER_MINUTE 2
#define LAYER_NOTIFICATION 3
Layer* layers[LAYERS_COUNT];

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

static void render_notifications(Layer *layer, GContext* ctx){
	renderers_notifications(layer,ctx);
}

static void render_center(Layer *layer, GContext* ctx){
	renderers_center(layer,ctx);
}

static void render_minute(Layer *layer, GContext* ctx){
	renderers_minute(layer,ctx);
}

static void render_hour(Layer *layer, GContext* ctx){
	renderers_hour(layer,ctx);	
}


void handle_tick(struct tm* tick_time, TimeUnits units_changed){
	if(tick_time==NULL){
		time_t now_time_t=time(NULL);	
		tick_time=localtime(&now_time_t);
	}
	//Texts
	renderers_text_hour(text_layers[TEXT_LAYER_HOUR],tick_time);
	renderers_text_minute(text_layers[TEXT_LAYER_MINUTE],tick_time);
	renderers_text_date(text_layers[TEXT_LAYER_DATE],tick_time);
	renderers_text_day(text_layers[TEXT_LAYER_DAY],tick_time);
	//Analog
	layer_mark_dirty(layers[LAYER_HOUR]);
	layer_mark_dirty(layers[LAYER_MINUTE]);
}

static void window_load(Window* window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	bitmap_layer=bitmap_layer_create(GRect((bounds.size.w-130)/2, (bounds.size.h-130)/2, 130, 130));
	bitmap_layer_set_bitmap(bitmap_layer,bitmaps_get_bitmap(RESOURCE_ID_BACKGROUND));
	layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
	
	text_layers[TEXT_LAYER_DAY] = builder_text_layer(GRect(0, -5, bounds.size.w, 28),window_layer,31,GTextAlignmentCenter,FONT_DAY);
	text_layers[TEXT_LAYER_DATE] = builder_text_layer(GRect(0, bounds.size.h-24, bounds.size.w, 24),window_layer,31,GTextAlignmentCenter,FONT_DATE);
	text_layers[TEXT_LAYER_HOUR] = builder_text_layer(GRect(-8, bounds.size.h/2-(46/2), bounds.size.w/2, 40),window_layer,3,GTextAlignmentRight,FONT_HOUR);
	text_layers[TEXT_LAYER_MINUTE] = builder_text_layer(GRect(bounds.size.w/2+8, bounds.size.h/2-(46/2), bounds.size.w/2, 40),window_layer,3,GTextAlignmentLeft,FONT_MINUTE);
	
	layers[LAYER_NOTIFICATION]=builder_layer(GRect(0, 0, bounds.size.w, bounds.size.h),window_layer,render_notifications);
	layers[LAYER_CENTER]=builder_layer(GRect(0, (bounds.size.h-bounds.size.w)/2, bounds.size.w, bounds.size.w),window_layer,render_center);
	layers[LAYER_HOUR]=builder_layer(GRect(0, 0, bounds.size.w, bounds.size.h),window_layer,render_hour);
	layers[LAYER_MINUTE]=builder_layer(GRect(0, 0, bounds.size.w, bounds.size.h),window_layer,render_minute);
	
	inverter_layer=inverter_layer_create(GRect(0,0, bounds.size.w, bounds.size.h));
	layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
}

static void window_appear(Window *window) {
	handle_tick(NULL,MINUTE_UNIT);	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void window_disappear(Window *window) {
	tick_timer_service_unsubscribe();
}


static void window_unload(Window* window){
	for(uint8_t i=0;i<TEXT_LAYERS_COUNT;i++){
		if(text_layers[i]!=NULL){
			char* buffer=(char*)text_layer_get_text(text_layers[i]);
			text_layer_destroy(text_layers[i]);
			text_layers[i]=NULL;
			free(buffer);
		}
	}
	for(uint8_t i=0;i<LAYERS_COUNT;i++){
		if(layers[i]!=NULL){
			layer_destroy(layers[i]);
			layers[i]=NULL;
		}
	}
	bitmap_layer_destroy(bitmap_layer);
	inverter_layer_destroy(inverter_layer);
}