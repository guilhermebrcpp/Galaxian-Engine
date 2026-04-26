#pragma once
#include <windows.h>

extern bool key_states[128];
void update_keys_check();
bool is_key_pressed(char key);
bool is_key_just_pressed(char key);
