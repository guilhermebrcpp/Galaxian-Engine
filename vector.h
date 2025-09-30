#pragma once

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
    void print(){
        std::cout<<"X: "<<x<<" Y:"<<y<<std::endl;
    }
};

