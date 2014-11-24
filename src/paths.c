#include <pebble.h>
#include "paths.h"

#define PATHS_COUNT 2
GPath* paths[PATHS_COUNT];
	
const GPathInfo HOUR_PATH_BASIC = {
  3,
  (GPoint []) {
    {0, -51},
	{-8, -41},
	{8, -41},
  }
};
const GPathInfo MINUTE_PATH_BASIC = {
  3,
  (GPoint []) {
    {0, -68},
	{-8, -50},
	{8, -50},
  }
};

void paths_init(){
	paths[PATH_HOUR] = gpath_create(&HOUR_PATH_BASIC);
	paths[PATH_MINUTE] = gpath_create(&MINUTE_PATH_BASIC);
}

GPath* paths_get(uint8_t i){
	return paths[i];
}

void paths_deinit(){
	for(uint8_t i=0;i<PATHS_COUNT;i++){
	  gpath_destroy(paths[i]);
	  paths[i]=NULL;
  }
}