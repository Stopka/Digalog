#include <pebble.h>
#include "renderers.h"
#include "paths.h"

void renderers_notifications(Layer *layer, GContext* ctx){
	GRect bounds = layer_get_bounds(layer);
	
}
	
void renderers_center(Layer *layer, GContext* ctx){
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 6);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, center, 5);
}

void renderers_minute(Layer *layer, GContext* ctx){
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

void renderers_hour(Layer *layer, GContext* ctx){
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

void format_text_time(TextLayer* layer,struct tm* time,size_t max,const char* fmt){
	char* buffer=(char*)text_layer_get_text(layer);
	strftime(buffer,max,fmt,time);
	layer_mark_dirty(text_layer_get_layer(layer));
}

void renderers_text_minute(TextLayer *layer,struct tm* time){
	format_text_time(layer,time,3,"%M");
}

void renderers_text_hour(TextLayer *layer,struct tm* time){
	format_text_time(layer,time,3,clock_is_24h_style()?"%H":"%I");
}

void renderers_text_day(TextLayer *layer,struct tm* time){
	format_text_time(layer,time,31,"%A");
}

void renderers_text_date(TextLayer *layer,struct tm* time){
	format_text_time(layer,time,31,"%d. %m. %Y");
}