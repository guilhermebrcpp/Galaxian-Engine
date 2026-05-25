#pragma once
#include <windows.h>

extern bool key_states[128];

// Need to run this every frame for the checks to work
void update_keys_check();
// Check if a key is being holded
bool is_key_pressed(char key);
// Check if a key was just pressed
bool is_key_just_pressed(char key);
