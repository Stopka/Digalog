#pragma once

#define PATH_HOUR 0
#define PATH_MINUTE 1

void paths_init();
GPath* paths_get(uint8_t i);
void paths_deinit();