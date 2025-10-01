#pragma once
#include "vector.h"

class camera{
public:
    vector3 pos;
    vector3 rotation;
    float fov;
    camera(){
        fov = 60;
        pos.set(0, 0, 0);
        rotation.set(0, 0, 0);
    }
};
