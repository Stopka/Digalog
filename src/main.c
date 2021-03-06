#include "bitmap-loader.h"
#include "window_main.h"
#include "paths.h"
#include "fonts.h"
#include "model.h"

void handle_init() {
	model_init();
	
	bitmaps_init();
	paths_init();
	fonts_init();
		
	window_main_init();
	
	window_main_show();
}

void handle_deinit() {
	window_main_deinit();
	
	paths_deinit();
	fonts_deinit();
	bitmaps_cleanup();
	
	model_init();
}

int main() {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}