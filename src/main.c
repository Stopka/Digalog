#include "bitmap-loader.h"
#include "window_main.h"
#include "paths.h"

void handle_init() {
	bitmaps_init();
	paths_init();
		
	window_main_init();
	
	window_main_show();
}

void handle_deinit() {
	window_main_deinit();
	
	paths_init();
	bitmaps_cleanup();
}

int main() {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}