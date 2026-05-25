#pragma once
#include <math.h>
#include <iostream>
#include <immintrin.h>

class vector3{
public:
    float x = 0;
    float y = 0;
    float z = 0;

    void set(float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void set(vector3 a){
        this->x = a.x;
        this->y = a.y;
        this->z = a.z;
    }

    void add(float x, float y, float z){
        this->x += x;
        this->y += y;
        this->z += z;
    }

    void add(vector3 a){
        this->x += a.x;
        this->y += a.y;
        this->z += a.z;
    }

    void sub(vector3 a){
        this->x -= a.x;
        this->y -= a.y;
        this->z -= a.z;
    }

    float dot(vector3 a){
        return x*a.x + y*a.y + z*a.z;
    }
    vector3 normalized(){
        vector3 normalized_vector;
        normalized_vector.set(x, y, z);
        float magnitude = std::sqrt(x * x + y * y + z * z);
        if(magnitude > 0){
            normalized_vector.x /= magnitude;
            normalized_vector.y /= magnitude;
            normalized_vector.z /= magnitude;
        }
        return normalized_vector;
    }
    void print(){
        std::cout<<"X: "<<x<<" Y: "<<y<<" Z :"<<z<<std::endl;
    }
};

class vector2{
public:
    float x = 0;
    float y = 0;

    void set(float x, float y){
        this->x = x;
        this->y = y;
    }
    void add(vector2 a){
        this->x += a.x;
        this->y += a.y;
    }

    void sub(vector2 a){
        this->x -= a.x;
        this->y -= a.y;
    }
    float dot(vector2 a){
        return x*a.x + y*a.y;
    }
    void print(){
        std::cout<<"X: "<<x<<" Y:"<<y<<std::endl;
    }
};

class vector2i{
public:
    int x = 0;
    int y = 0;

    void set(int x, int y){
        this->x = x;
        this->y = y;
    }

    void add(vector2 a){
        this->x += a.x;
        this->y += a.y;
    }

    void sub(vector2 a){
        this->x -= a.x;
        this->y -= a.y;
    }

    void print(){
        std::cout<<"X: "<<x<<" Y:"<<y<<std::endl;
    }
};

vector3 rotated_by_x(vector3 point, float rotation);
vector3 rotated_by_y(vector3 point, float rotation);
vector3 rotated_by_z(vector3 point, float rotation);
void matrix_mult4x4(const float mat1[][4], const float mat2[][4], float out[][4]);
void matrix_mult4x1(const float mat1[][4], const float mat2[4], float out[4]);
void calculate_local_to_world_matrix(vector3 position, vector3 rotation, vector3 scale, float out[][4]);
void calculate_world_to_view_matrix(vector3 position, vector3 rotation, float out[][4]);
void calculate_perspective_mtx(float aspect_ratio, float fov, float near, float far, float out[][4]);
void matrix_mult4x1_simd(__m256 _mtx[4][4], __m256 _final_points[4]);
void load_values_simd_mtx(float mtx[][4], __m256 simd_mtx[][4]);
