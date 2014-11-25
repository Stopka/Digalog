#pragma once
#include "model.h"
void renderers_notifications(Layer *layer, GContext* ctx);
void renderers_hour(Layer *layer, GContext* ctx);
void renderers_minute(Layer *layer, GContext* ctx);
void renderers_center(Layer *layer, GContext* ctx);

void renderers_text_minute(TextLayer *layer,struct tm* time);
void renderers_text_hour(TextLayer *layer,struct tm* time);
void renderers_text_day(TextLayer *layer,struct tm* time);
void renderers_text_date(TextLayer *layer,struct tm* time);