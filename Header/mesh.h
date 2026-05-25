#pragma once
#include <vector>
#include "vector.h"
#include <fstream>
#include "texture.h"
#include "material.h"

class mesh{
public:
    //need them separated for later optimizations using SIMD (this makes me very sad)
    std::vector <float> vertices_x;
    std::vector <float> vertices_y;
    std::vector <float> vertices_z;
    std::vector <std::vector<int>> triangles;
    std::vector <std::string> materials_used_in_order;
    std::vector <vector2> vertex_texture;
    std::vector <std::vector<int>> vertex_texture_indices;
    vector3 pos;
    vector3 rotation;
    vector3 scale;
    std::vector<texture> mesh_texture;
    std::vector<material> materials;
    bool has_texture = false;

    mesh(){
        pos.set(0, 0, 0);
        rotation.set(0, 0, 0);
        scale.set(1, 1, 1);
    }

    void set_texture(texture tex){
        mesh_texture.push_back(tex);
        has_texture = true;
    }

    bool have_texture(){
        return has_texture;
    }

    int get_current_material(int sub_mesh_index){
        for(int i = 0; i < materials.size(); i++){
            if(materials_used_in_order[sub_mesh_index] == materials[i].name){
                return i;
            }
        }
        return -1;
    }
};
