#pragma once
#include "vector.h"

class camera{
public:
    vector3 pos;
    vector3 rotation;
    float fov;
    float cam_near;
    float cam_far;

    camera(){
        cam_near = 0.1;
        cam_far = 100;
        fov = 80;
        pos.set(0, 0, 0);
        //rotate camera by 180 in the z axis to invert final image
        rotation.set(0, 0, 3.14159/2);
    }
};
