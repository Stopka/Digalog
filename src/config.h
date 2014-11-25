#pragma once
typedef enum {
  CONFIG_VIBRATE_NONE,
	CONFIG_VIBRATE_SHORT,
	CONFIG_VIBRATE_LONG,
	CONFIG_VIBRATE_DOUBLE,
} ConfigVibrate;

typedef struct ConfigNotify {
  ConfigVibrate vibrate;
	bool light;
} __attribute__((__packed__)) ConfigNotify;

typedef struct ConfigValue {
  ConfigNotify notify;
	bool show;
} __attribute__((__packed__)) ConfigValue;
	
typedef struct ConfigBool {
  ConfigValue on;
	ConfigValue off;
} __attribute__((__packed__)) ConfigBool;

typedef struct ConfigBattery {
	ConfigNotify notify;
  int8_t low;
	int8_t show;
	bool plugged;
	bool charging;
} __attribute__((__packed__)) ConfigBattery;

typedef struct ConfigPower {
  ConfigBool plugged;
	ConfigValue charging;
} __attribute__((__packed__)) ConfigPower;

typedef struct Config {
	bool black;
  ConfigBool bluetooth;
	ConfigBattery battery;
	ConfigPower power;
	ConfigNotify hour;
} __attribute__((__packed__)) Config;

Config config_get();
void config_init();
void config_deinit();