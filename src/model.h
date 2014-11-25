#pragma once
	
void model_init();
void model_deinit();

typedef enum {
  MODEL_BLUETOOTH_CONNECTED,
	MODEL_BLUETOOTH_NONE,
	MODEL_BLUETOOTH_DISCONNECTED
} ModelBluetooth;

typedef enum {
  MODEL_POWER_NONE,
	MODEL_POWER_CHARGING,
	MODEL_POWER_PLUGGED,
	MODEL_POWER_UNPLUGGED
} ModelPower;

ModelBluetooth model_bluetooth();
int8_t model_battery();
ModelPower model_power();

void model_handle_appear();
struct tm* model_handle_tick(struct tm* time,TimeUnits units);
void model_handle_bluetooth(bool connected);
void model_handle_battery(BatteryChargeState charge);