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

void matrix_mult4x4(const float mat1[][4], const float mat2[][4], float out[][4]){
    float temp[4][4];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
            temp[i][j] = 0;
            for(int l = 0; l < 4; l++){
                temp[i][j] += mat1[i][l] * mat2[l][j];
            }
		}
	}
	memcpy(out, temp, sizeof(temp));
}

void matrix_mult4x1(const float mat1[][4], const float mat2[4], float out[4]){
    float temp[4];
    for(int i = 0; i < 4; i++){
        temp[i] = 0;
        for(int l = 0; l < 4; l++){
            temp[i] += mat1[i][l] * mat2[l];
        }
    }
    memcpy(out, temp, sizeof(temp));
}

void calculate_local_to_world_matrix(vector3 position, vector3 rotation, vector3 scale, float out[][4]){
    float rotationx_cos = cos(rotation.x);
    float rotationx_sin = sin(rotation.x);
    float rotation_x[4][4] = {
        {1, 0, 0, 0},
        {0, rotationx_cos, -rotationx_sin, 0},
        {0, rotationx_sin, rotationx_cos, 0},
        {0, 0, 0, 1}
    };

    float rotationy_cos = cos(rotation.y);
    float rotationy_sin = sin(rotation.y);
    float rotation_y[4][4] = {
        {rotationy_cos, 0, rotationy_sin, 0},
        {0, 1, 0, 0},
        {-rotationy_sin, 0, rotationy_cos, 0},
        {0, 0, 0, 1}
    };

    float rotationz_cos = cos(rotation.z);
    float rotationz_sin = sin(rotation.z);
    float rotation_z[4][4] = {
        {rotationz_cos, -rotationz_sin, 0, 0},
        {rotationz_sin, rotationz_cos, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float scale_mtx[4][4] = {
        {scale.x, 0, 0, 0},
        {0, scale.y, 0, 0},
        {0, 0, scale.z, 0},
        {0, 0, 0, 1}
    };

    float translation_mtx[4][4] = {
        {1, 0, 0, position.x},
        {0, 1, 0, position.y},
        {0, 0, 1, position.z},
        {0, 0, 0, 1}
    };

    float local_to_world_mtx[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    matrix_mult4x4(scale_mtx, rotation_z, local_to_world_mtx);
    matrix_mult4x4(rotation_y, local_to_world_mtx, local_to_world_mtx);
    matrix_mult4x4(rotation_x, local_to_world_mtx, local_to_world_mtx);
    matrix_mult4x4(translation_mtx, local_to_world_mtx, local_to_world_mtx);
    memcpy(out, local_to_world_mtx, sizeof(local_to_world_mtx));
}


void calculate_world_to_view_matrix(vector3 position, vector3 rotation, float out[][4]){
    float rotationx_cos = cos(-rotation.x);
    float rotationx_sin = sin(-rotation.x);
    float rotation_x[4][4] = {
        {1, 0, 0, 0},
        {0, rotationx_cos, -rotationx_sin, 0},
        {0, rotationx_sin, rotationx_cos, 0},
        {0, 0, 0, 1}
    };

    float rotationy_cos = cos(-rotation.y);
    float rotationy_sin = sin(-rotation.y);
    float rotation_y[4][4] = {
        {rotationy_cos, 0, rotationy_sin, 0},
        {0, 1, 0, 0},
        {-rotationy_sin, 0, rotationy_cos, 0},
        {0, 0, 0, 1}
    };

    float rotationz_cos = cos(rotation.z);
    float rotationz_sin = sin(rotation.z);
    float rotation_z[4][4] = {
        {rotationz_cos, -rotationz_sin, 0, 0},
        {rotationz_sin, rotationz_cos, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float scale_mtx[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float translation_mtx[4][4] = {
        {1, 0, 0, -position.x},
        {0, 1, 0, -position.y},
        {0, 0, 1, -position.z},
        {0, 0, 0, 1}
    };

    float local_to_world_mtx[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    matrix_mult4x4(translation_mtx, rotation_z, local_to_world_mtx);
    matrix_mult4x4(rotation_y, local_to_world_mtx, local_to_world_mtx);
    matrix_mult4x4(rotation_x, local_to_world_mtx, local_to_world_mtx);
    memcpy(out, local_to_world_mtx, sizeof(local_to_world_mtx));
}
