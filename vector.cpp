#include "vector.h"
#include <math.h>

vector3 rotated_by_x(vector3 point, float rotation){
    vector3 rotationxx; rotationxx.set(1, 0, 0);
    vector3 rotationxy; rotationxy.set(0, cos(rotation), -sin(rotation));
    vector3 rotationxz; rotationxz.set(0, sin(rotation), cos(rotation));

    vector3 final_point;

    final_point.x = point.x*rotationxx.x + point.y*rotationxx.y + point.z*rotationxx.z;
    final_point.y = point.x*rotationxy.x + point.y*rotationxy.y + point.z*rotationxy.z;
    final_point.z = point.x*rotationxz.x + point.y*rotationxz.y + point.z*rotationxz.z;

    return final_point;
}

vector3 rotated_by_y(vector3 point, float rotation){
    //y rotation
    vector3 rotationyx; rotationyx.set(cos(rotation), 0, sin(rotation));
    vector3 rotationyy; rotationyy.set(0, 1, 0);
    vector3 rotationyz; rotationyz.set(-sin(rotation), 0, cos(rotation));

    vector3 final_point;

    final_point.x = point.x*rotationyx.x + point.y*rotationyx.y + point.z*rotationyx.z;
    final_point.y = point.x*rotationyy.x + point.y*rotationyy.y + point.z*rotationyy.z;
    final_point.z = point.x*rotationyz.x + point.y*rotationyz.y + point.z*rotationyz.z;

    return final_point;
}

vector3 rotated_by_z(vector3 point, float rotation){
    //y rotation
    vector3 rotationzx; rotationzx.set(cos(rotation),-sin(rotation), 0);
    vector3 rotationzy; rotationzy.set(sin(rotation), cos(rotation), 0);
    vector3 rotationzz; rotationzz.set(0, 0, 1);

    vector3 final_point;

    final_point.x = point.x*rotationzx.x + point.y*rotationzx.y + point.z*rotationzx.z;
    final_point.y = point.x*rotationzy.x + point.y*rotationzy.y + point.z*rotationzy.z;
    final_point.z = point.x*rotationzz.x + point.y*rotationzz.y + point.z*rotationzz.z;

    return final_point;
}
