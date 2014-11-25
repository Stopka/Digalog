#include <pebble.h>
#include "model.h"
#include "config.h"
bool bluetooth;
BatteryChargeState battery;

void notify(ConfigNotify notification){
	if(notification.light){
		light_enable_interaction();
	}
	if(notification.vibrate==CONFIG_VIBRATE_SHORT){
		vibes_short_pulse();
	}
	if(notification.vibrate==CONFIG_VIBRATE_LONG){
		vibes_long_pulse();
	}
	if(notification.vibrate==CONFIG_VIBRATE_DOUBLE){
		vibes_double_pulse();
	}
}

void model_handle_bluetooth(bool connected){
	Config config=config_get();
	if(bluetooth!=connected){
		if(connected){
			notify(config.bluetooth.on.notify);
		}else{
			notify(config.bluetooth.off.notify);
		}
	}
	bluetooth=connected;
}

void model_handle_battery(BatteryChargeState charge){
	Config config=config_get();
	if(battery.charge_percent!=charge.charge_percent){
		if(charge.charge_percent<=config.battery.low&&battery.charge_percent>config.battery.low){
			notify(config.battery.notify);
		}
	}
	if(battery.is_plugged!=charge.is_plugged){
		if(charge.is_plugged){
			notify(config.power.plugged.on.notify);
		}else{
			notify(config.power.plugged.off.notify);
		}
	}
	if(battery.is_charging!=charge.is_charging){
		if(!charge.is_charging&&charge.is_plugged){
			notify(config.power.charging.notify);
		}
	}
	battery=charge;
}

struct tm* model_handle_tick(struct tm* newtime,TimeUnits units){
	if(newtime==NULL){
		time_t now_time_t=time(NULL);	
		newtime=localtime(&now_time_t);
	}
	
	return newtime;
}

ModelBluetooth model_bluetooth(){
	Config config=config_get();
	if(bluetooth&&config.bluetooth.on.show){
		return MODEL_BLUETOOTH_CONNECTED;
	}
	if(!bluetooth&&config.bluetooth.off.show){
		return MODEL_BLUETOOTH_DISCONNECTED;
	}
	return MODEL_BLUETOOTH_NONE;
}

int8_t model_battery(){
	Config config=config_get();
	if( (battery.charge_percent<=config.battery.show) || (battery.is_charging&&config.battery.charging) || (battery.is_plugged&&config.battery.plugged)){
		return battery.charge_percent;
	}
	return -1;
}

ModelPower model_power(){
	Config config=config_get();
	if( battery.is_charging&&config.power.charging.show){
		return MODEL_POWER_CHARGING;
	}
	if( battery.is_plugged&&config.power.plugged.on.show ){
		return MODEL_POWER_PLUGGED;
	}
	if( !battery.is_plugged&&config.power.plugged.off.show ){
		return MODEL_POWER_UNPLUGGED;
	}
	return MODEL_POWER_NONE;
}

void model_handle_appear(){
	bluetooth=bluetooth_connection_service_peek();
	battery=battery_state_service_peek();
}

void model_init(){
	config_init();
	model_handle_appear();
}

void model_deinit(){
	config_deinit();
}