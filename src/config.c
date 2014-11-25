#include <pebble.h>
#include "config.h"
Config config;
	
void config_set_default(){
	config.black=true;
	
	config.bluetooth.off.notify.light=true;
	config.bluetooth.off.notify.vibrate=CONFIG_VIBRATE_DOUBLE;
	config.bluetooth.off.show=true;
	
	config.battery.low=-1;
	config.battery.show=15;
	config.battery.plugged=true;
	config.battery.charging=true;
	
	//config.power.plugged.on.show=true;
	config.power.charging.show=true;
}

Config config_get(){
	return config;
}

void config_init(){
	config_set_default();
	//TODO load config
}

void config_deinit(){

}