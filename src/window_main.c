#include <pebble.h>
#include "window_main.h"
#include "bitmap-loader.h"
#include "paths.h"

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
BitmapLayer* bitmap_layer;

InverterLayer* inverter_layer;

#define LAYERS_COUNT 3
#define LAYER_CENTER 0
#define LAYER_HOUR 1
#define LAYER_MINUTE 2
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

static void render_center(Layer *layer, GContext* ctx){
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 6);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, center, 5);
}

static void render_minute(Layer *layer, GContext* ctx){
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
  unsigned int angle = t->tm_min*6;
	GPath* path=paths_get(PATH_MINUTE);
	gpath_move_to(path, center);
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_rotate_to(path, angle*TRIG_MAX_ANGLE/360);
	gpath_draw_filled(ctx, path);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	gpath_draw_outline(ctx, path);
	
}

static void render_hour(Layer *layer, GContext* ctx){
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
  unsigned int angle = ((t->tm_hour % 12) * 30)+ (t->tm_min/2);
	GPath* path=paths_get(PATH_HOUR);
	gpath_move_to(path, center);
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_rotate_to(path, angle*TRIG_MAX_ANGLE/360);
	gpath_draw_filled(ctx, path);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	gpath_draw_outline(ctx, path);
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
	
	bitmap_layer=bitmap_layer_create(GRect((bounds.size.w-130)/2, (bounds.size.h-130)/2, 130, 130));
	bitmap_layer_set_bitmap(bitmap_layer,bitmaps_get_bitmap(RESOURCE_ID_BACKGROUND));
	layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
	
	text_layer_buffers[TEXT_LAYER_DAY]=(char *)malloc(31*sizeof(char));
	text_layers[TEXT_LAYER_DAY] = text_layer_create(GRect(0, -7, bounds.size.w, 28));
  text_layer_set_text(text_layers[TEXT_LAYER_DAY], text_layer_buffers[TEXT_LAYER_DAY]);
	text_layer_set_font(text_layers[TEXT_LAYER_DAY],fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_DAY],GTextAlignmentCenter);
	text_layer_set_background_color(text_layers[TEXT_LAYER_DAY],GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_DAY]));
	
	text_layer_buffers[TEXT_LAYER_DATE]=(char *)malloc(31*sizeof(char));
	text_layers[TEXT_LAYER_DATE] = text_layer_create(GRect(0, bounds.size.h-26, bounds.size.w, 24));
  text_layer_set_text(text_layers[TEXT_LAYER_DATE], text_layer_buffers[TEXT_LAYER_DATE]);
	text_layer_set_font(text_layers[TEXT_LAYER_DATE],fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_DATE],GTextAlignmentCenter);
	text_layer_set_background_color(text_layers[TEXT_LAYER_DATE],GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_DATE]));
	
	text_layer_buffers[TEXT_LAYER_HOUR]=(char *)malloc(3*sizeof(char));
	text_layers[TEXT_LAYER_HOUR] = text_layer_create(GRect((bounds.size.w-120)/2, bounds.size.h/2-(44/2), 120/2, 34));
  text_layer_set_text(text_layers[TEXT_LAYER_HOUR], text_layer_buffers[TEXT_LAYER_HOUR]);
	text_layer_set_font(text_layers[TEXT_LAYER_HOUR],fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_HOUR],GTextAlignmentCenter);
	text_layer_set_background_color(text_layers[TEXT_LAYER_HOUR],GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_HOUR]));
	
	text_layer_buffers[TEXT_LAYER_MINUTE]=(char *)malloc(3*sizeof(char));
	text_layers[TEXT_LAYER_MINUTE] = text_layer_create(GRect(bounds.size.w/2, bounds.size.h/2-(44/2), 120/2, 34));
  text_layer_set_text(text_layers[TEXT_LAYER_MINUTE], text_layer_buffers[TEXT_LAYER_MINUTE]);
	text_layer_set_font(text_layers[TEXT_LAYER_MINUTE],fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_MINUTE],GTextAlignmentCenter);
	text_layer_set_background_color(text_layers[TEXT_LAYER_MINUTE],GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_MINUTE]));
		
	layers[LAYER_CENTER]=layer_create(GRect(0, (bounds.size.h-bounds.size.w)/2, bounds.size.w, bounds.size.w));
	layer_set_clips	(layers[LAYER_CENTER],false);
	layer_set_update_proc(layers[LAYER_CENTER], render_center);
	layer_add_child(window_layer,layers[LAYER_CENTER]);
	
	layers[LAYER_HOUR]=layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_clips	(layers[LAYER_HOUR],false);
	layer_set_update_proc(layers[LAYER_HOUR], render_hour);
	layer_add_child(window_layer,layers[LAYER_HOUR]);
	
	layers[LAYER_MINUTE]=layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_clips	(layers[LAYER_MINUTE],false);
	layer_set_update_proc(layers[LAYER_MINUTE], render_minute);
	layer_add_child(window_layer,layers[LAYER_MINUTE]);
	
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
	for(uint8_t i=0;i<LAYERS_COUNT;i++){
		if(layers[i]!=NULL){
			layer_destroy(layers[i]);
			layers[i]=NULL;
		}
	}
	bitmap_layer_destroy(bitmap_layer);
	inverter_layer_destroy(inverter_layer);
}