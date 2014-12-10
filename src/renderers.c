#include <pebble.h>
#include "renderers.h"
#include "paths.h"
#include "bitmap-loader.h"

void renderers_notification_bluetooth(GRect bounds, GContext* ctx){
	ModelBluetooth state=model_bluetooth();
	switch(state){
		case MODEL_BLUETOOTH_CONNECTED:
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_BLUETOOTH),
				GRect(bounds.origin.x+(bounds.size.w-9)/2,bounds.origin.y+(bounds.size.h-16)/2,9,16)
			);
			return;
		case MODEL_BLUETOOTH_DISCONNECTED:
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_BLUETOOTH),
				GRect(bounds.origin.x+0,bounds.origin.y+(bounds.size.h-16)/2,9,16)
			);
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_NONE),
				GRect(bounds.origin.x+bounds.size.w-9,bounds.origin.y+(bounds.size.h-9)/2,9,9)
			);
			return;
		default:
			return;
	}
}

void renderers_notification_battery(GRect bounds, GContext* ctx){
	int8_t state=model_battery();
	if(state<0){
		return;
	}
	graphics_draw_bitmap_in_rect(ctx,
		bitmaps_get_bitmap(RESOURCE_ID_BATTERY),
		GRect(bounds.origin.x+(bounds.size.w-15)/2,bounds.origin.y+(bounds.size.h-8)/2,15,8)
	);
	graphics_fill_rect(ctx,
		GRect((bounds.origin.x+(bounds.size.w-15)/2)+2,(bounds.origin.y+(bounds.size.h-8)/2)+2,10*state/100,4),
		0,GCornerNone
	);
}

void renderers_notification_power(GRect bounds, GContext* ctx){
	ModelPower state=model_power();
	switch(state){
		case MODEL_POWER_CHARGING:
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_CHARGING),
				GRect(bounds.origin.x+(bounds.size.w-6)/2,bounds.origin.y+(bounds.size.h-10)/2,6,10)
			);
			return;
		case MODEL_POWER_PLUGGED:
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_PLUGGED),
				GRect(bounds.origin.x+(bounds.size.w-9)/2,bounds.origin.y+(bounds.size.h-6)/2,9,6)
			);
			return;
		case MODEL_POWER_UNPLUGGED:
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_PLUGGED),
				GRect(bounds.origin.x+(bounds.size.w-14)/2,bounds.origin.y+(bounds.size.h-6)/2,9,6)
			);
			graphics_draw_bitmap_in_rect(	ctx,
				bitmaps_get_bitmap(RESOURCE_ID_NONE),
				GRect(bounds.origin.x+bounds.size.w-9,bounds.origin.y+(bounds.size.h-9)/2,9,9)
			);
			return;
		default:
			return;
	}
}
	
void renderers_notifications(Layer *layer, GContext* ctx){
	GRect bounds = layer_get_bounds(layer);
	if(model_power()!=MODEL_POWER_NONE&&model_battery()>=0){
		renderers_notification_power(GRect((bounds.size.w-34)/2,(bounds.size.h-16)/2-33,16,16), ctx);
		renderers_notification_battery(GRect((bounds.size.w-34)/2+18,(bounds.size.h-16)/2-33,16,16), ctx);
	}else{
		renderers_notification_battery(GRect((bounds.size.w-16)/2,(bounds.size.h-16)/2-33,16,16), ctx);
		renderers_notification_power(GRect((bounds.size.w-16)/2,(bounds.size.h-16)/2-33,16,16), ctx);
	}
	renderers_notification_bluetooth(GRect((bounds.size.w-16)/2,(bounds.size.h-16)/2+33,16,16), ctx);
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