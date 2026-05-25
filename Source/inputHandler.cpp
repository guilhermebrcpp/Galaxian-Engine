#include "inputHandler.h"

bool key_states[128];
bool previous_key_states[128];

void update_keys_check(){
    for(int i = 0; i < 128; i++){
        previous_key_states[i] = key_states[i];
        key_states[i] = GetAsyncKeyState(i);
    }
}

bool is_key_pressed(char key){
    if(int(key) < 0 || int(key) > 127) return false;

    return key_states[int(key)];
}

bool is_key_just_pressed(char key){
    if(int(key) < 0 || int(key) > 127) return false;

    return (key_states[int(key)] == 1 && previous_key_states[int(key)] == 0);
}
