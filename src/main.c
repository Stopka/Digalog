#include <pebble.h>
#define FONT RESOURCE_ID_FONT_22
#define FONT_BOLD RESOURCE_ID_FONT_BOLD_38
	
Window *window;
BitmapLayer* analog_background;
BitmapLayer* analog_center[2];
BitmapLayer* notifications[2];
RotBitmapLayer* analog_hands[4];
int battery_state=0;
Layer* analog;
Layer* digital_time;
#define COUNT_TEXTS 4
TextLayer* texts[COUNT_TEXTS];
#define COUNT_BITMAPS 7
GBitmap* bitmaps[COUNT_BITMAPS];//0=background,1+2=center,3+4=hour,5+6=minute
#define COUNT_FONTS 2
GFont fonts[COUNT_FONTS];//0=time,1=rest
///////////////////////////
// Window handlers
///////////////////////////
static void window_load(Window *window) {
	//Analog
	analog=layer_create	(GRect(7, 19, 130, 130));
	
	bitmaps[0]=gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	bitmaps[1]=gbitmap_create_with_resource(RESOURCE_ID_CENTER_WHITE);
	bitmaps[2]=gbitmap_create_with_resource(RESOURCE_ID_CENTER_BLACK);
	bitmaps[3]=gbitmap_create_with_resource(RESOURCE_ID_HOUR_WHITE);
	bitmaps[4]=gbitmap_create_with_resource(RESOURCE_ID_HOUR_BLACK);
	bitmaps[5]=gbitmap_create_with_resource(RESOURCE_ID_MINUTE_WHITE);
	bitmaps[6]=gbitmap_create_with_resource(RESOURCE_ID_MINUTE_BLACK);
	
	fonts[0]=fonts_load_custom_font(resource_get_handle(FONT_BOLD));
	fonts[1]=fonts_load_custom_font(resource_get_handle(FONT));
	
	analog_background=bitmap_layer_create(GRect(0, 0, 130, 130));
	bitmap_layer_set_bitmap(analog_background,bitmaps[0]);	
	layer_add_child(analog,(Layer*)analog_background);
	
	notifications[0]=bitmap_layer_create(GRect(57, 24, 16, 16));
	layer_add_child(analog,(Layer*)notifications[0]);
	notifications[1]=bitmap_layer_create(GRect(57, 90, 16, 16));
	layer_add_child(analog,(Layer*)notifications[1]);
	
	analog_center[0]=bitmap_layer_create(GRect(59, 59, 12, 12));
	analog_center[1]=bitmap_layer_create(GRect(59, 59, 12, 12));
	bitmap_layer_set_bitmap(analog_center[0],bitmaps[1]);
	bitmap_layer_set_bitmap(analog_center[1],bitmaps[2]);
	bitmap_layer_set_compositing_mode(analog_center[0], GCompOpOr);
	bitmap_layer_set_compositing_mode(analog_center[1], GCompOpClear);
	layer_add_child(analog,(Layer*)analog_center[0]);
	layer_add_child(analog,(Layer*)analog_center[1]);
	
	layer_add_child(window_get_root_layer(window), (Layer*)analog);
	
	//Digital time
	digital_time=layer_create(GRect(0, 41, 130, 100));
	layer_add_child((Layer*)analog, (Layer*)digital_time);
	
	texts[0]=text_layer_create(GRect(6, 0, 59, 100));
	text_layer_set_font(texts[0],fonts[0]);
	text_layer_set_background_color	(texts[0],GColorClear);
	text_layer_set_text_color(texts[0],GColorWhite);
	text_layer_set_text_alignment(texts[0], GTextAlignmentCenter);
	text_layer_set_text	(texts[0],"22");
	layer_add_child((Layer *)digital_time, (Layer *)texts[0]);
	
	texts[1]=text_layer_create(GRect(65, 0, 59, 100));
	text_layer_set_font(texts[1],fonts[0]);
	text_layer_set_background_color	(texts[1],GColorClear);
	text_layer_set_text_color(texts[1],GColorWhite);
	text_layer_set_text_alignment(texts[1], GTextAlignmentCenter);
	text_layer_set_text	(texts[1],"11");
	layer_add_child((Layer *)digital_time, (Layer *)texts[1]);
	
	//date
	texts[2]=text_layer_create(GRect(0, -4, 144, 100));
	text_layer_set_font(texts[2],fonts[1]);
	text_layer_set_background_color	(texts[2],GColorClear);
	text_layer_set_text_color(texts[2],GColorWhite);
	text_layer_set_text_alignment(texts[2], GTextAlignmentCenter);
	text_layer_set_text	(texts[2],"Wednesday");
	layer_add_child(window_get_root_layer(window), (Layer *)texts[2]);
	
	texts[3]=text_layer_create(GRect(0, 168-23, 144, 100));
	text_layer_set_font(texts[3],fonts[1]);
	text_layer_set_background_color	(texts[3],GColorClear);
	text_layer_set_text_color(texts[3],GColorWhite);
	text_layer_set_text_alignment(texts[3], GTextAlignmentCenter);
	text_layer_set_text	(texts[3],"00.00.0000");
	layer_add_child(window_get_root_layer(window), (Layer *)texts[3]);
}

static void update_time(struct tm *tick_time, TimeUnits units_changed){
	strftime((char*)text_layer_get_text(texts[0]),3,clock_is_24h_style()?"%H":"%I",tick_time);
	layer_mark_dirty((Layer *)texts[0]);
	strftime((char*)text_layer_get_text(texts[1]),3,"%M",tick_time);
	layer_mark_dirty((Layer *)texts[1]);
	strftime((char*)text_layer_get_text(texts[2]),10,"%A",tick_time);
	layer_mark_dirty((Layer *)texts[2]);
	strftime((char*)text_layer_get_text(texts[3]),11,"%d.%m.%Y",tick_time);
	layer_mark_dirty((Layer *)texts[3]);
}

static void update_bluetooth(bool connected) {
    GBitmap* btm=(GBitmap*)bitmap_layer_get_bitmap(notifications[1]);
	if(btm!=NULL&&connected){//prave pripojeno
		bitmap_layer_set_bitmap(notifications[1],NULL);
		gbitmap_destroy(btm);
	}
	if(btm==NULL&&!connected){//prave odpojeno?
		bitmap_layer_set_bitmap(notifications[1],gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_NONE));
		vibes_long_pulse();
	}
	layer_mark_dirty((Layer *)notifications[1]);
}

int getBatteryState(BatteryChargeState charge_state){
	if(charge_state.is_charging){
		if(charge_state.charge_percent==100){
			return 1;
		}
		if(charge_state.charge_percent>=50){
			return 2;
		}
		return 3;
	}
	if(charge_state.charge_percent<5){
		return 4;
	}
	if(charge_state.charge_percent<10){
		return 5;
	}
	return 0;
}

static void update_battery(BatteryChargeState charge_state) {
    int state=getBatteryState(charge_state);
	if(state==battery_state){
		return;
	}
	GBitmap* btm=(GBitmap*)bitmap_layer_get_bitmap(notifications[0]);
	bitmap_layer_set_bitmap(notifications[0],NULL);
	gbitmap_destroy(btm);
	switch(state){
		case 0:
			btm=NULL;
			break;
		case 1:
			btm=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_FULL);
			break;
		case 2:
			btm=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGING_HALF);
			break;
		case 3:
			btm=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGING_LOW);
			break;
		case 4:
			btm=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_EMPTY);
			break;
		case 5:
			btm=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_LOW);
			break;
	}
	bitmap_layer_set_bitmap(notifications[0],btm);
	battery_state=state;
	layer_mark_dirty((Layer *)notifications[0]);
}

static void window_appear(Window *window) {
	//Time
	time_t now_time_t=time(NULL);	
	struct tm * now_tm=localtime(&now_time_t);
	update_time(now_tm,SECOND_UNIT);
	tick_timer_service_subscribe(MINUTE_UNIT, update_time);
	//Bluetooth
	update_bluetooth(bluetooth_connection_service_peek());
	bluetooth_connection_service_subscribe(update_bluetooth);
	//Battery
	update_battery(battery_state_service_peek());
	battery_state_service_subscribe(update_battery);
}

static void window_disappear(Window *window) {
	tick_timer_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	battery_state_service_unsubscribe();
}

static void window_unload(Window *window) {
	//Analog
	layer_destroy(analog);
	layer_destroy(digital_time);
	
	for(int i=0;i<COUNT_TEXTS;i++){
		if(texts[i]!=NULL){
			text_layer_destroy(texts[i]);
		}
	}
	
	bitmap_layer_destroy(analog_background);
	bitmap_layer_destroy(analog_center[0]);
	bitmap_layer_destroy(analog_center[1]);
	
	for(int i=0;i<2;i++){
		GBitmap* btm=(GBitmap*)bitmap_layer_get_bitmap(notifications[i]);
		bitmap_layer_destroy(notifications[i]);
		if(btm!=NULL){
			gbitmap_destroy(btm);	
		}
	}

	for(int i=0;i<4;i++){
		rot_bitmap_layer_destroy(analog_hands[i]);
	}
	for(int i=0;i<COUNT_BITMAPS;i++){
		if(bitmaps[i]!=NULL){
			gbitmap_destroy(bitmaps[i]);
		}
	}
	
	for(int i=0;i<COUNT_FONTS;i++){
		fonts_unload_custom_font(fonts[i])	;
	}
}


///////////////////////////
//Base app calls
///////////////////////////
void handle_init(void) {
	window = window_create();
	window_set_background_color	(window,GColorBlack);	
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
	window_stack_push(window, true);
}

void handle_deinit(void) {
	window_destroy(window);
}

int main(void) {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
