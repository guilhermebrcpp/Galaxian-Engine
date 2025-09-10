#include <iostream>
#include <vector>
#include "vector.h"
#include "screen.h"
#include "renderer.h"
#include <math.h>
#include "mesh.h"

vector2 convert_3d_to_2d(vector3 a, float fov){
    vector2 new_point;
    new_point.x = (fov * a.x) / (fov + a.z);
    new_point.y = (fov * a.y) / (fov + a.z);

    return new_point;
}

void draw_points(screen* s, const std::vector<float> points, vector3 pos){
    std::cout<<"o tamanho do bagulho eh:"<<points.size()<<std::endl;
        for(int i = 0; i < points.size(); i+=3){
            //std::cout<<"i:"<<i<<std::endl;
            vector3 vec3;
            vec3.x =  points[i+0];
            vec3.y = -points[i+1];
            vec3.z =  points[i+2];
            vec3.add(pos);
            vector2 vec2 = convert_3d_to_2d(vec3, 40);
            s->draw_pixel(vec2.x, vec2.y, '#');
            std::cout<<"DESENHEI UM PONTO"<<std::endl;
        }
    }

float triangle_area(vector2 a, vector2 b, vector2 c){
//(1/2) (( x1(y2 − y3) + x2(y3 − y1) + x3(y1 − y2)))
    return (abs( (a.x*(b.y - c.y)) + (b.x*(c.y - a.y)) + (c.x*(a.y - b.y))))/2;
}

bool is_point_on_triangle(vector2 p, vector2 tri[]){
    float tri_area = triangle_area(tri[0], tri[1], tri[2]);

    float area1 = triangle_area(tri[0], tri[1], p);
    float area2 = triangle_area(tri[1], tri[2], p);
    float area3 = triangle_area(tri[2], tri[0], p);

    return ((area1 + area2 + area3)-0.1 < tri_area) && tri_area > 0;
}

void draw_triangle(screen* s, vector2 a, vector2 b, vector2 c, char color){
    int bounding_box_x_min = std::min(std::min(a.x, b.x), c.x);
    int bounding_box_x_max = std::max(std::max(a.x, b.x), c.x);
    int bounding_box_y_min = std::min(std::min(a.y, b.y), c.y);
    int bounding_box_y_max = std::max(std::max(a.y, b.y), c.y);

    vector2 tri[3] = {a, b, c};

    //std::cout<<"AREA DO BOUNDING BOX:"<<(bounding_box_x_max-bounding_box_x_min) * (bounding_box_y_max-bounding_box_y_min)<<std::endl;
    //a.print();
    //b.print();
    //c.print();

    for(int i = bounding_box_y_min; i < bounding_box_y_max; i++){
        for(int j = bounding_box_x_min; j < bounding_box_x_max; j++){
            vector2 point;
            point.set(j, i);
            if(is_point_on_triangle(point, tri)){
                s->draw_pixel(j, i, color);
                //std::cout<<"desenhei 1 pixel"<<std::endl;
            }
        }
    }
}

void render_mesh(screen* s, mesh m){
    std::vector<vector2> converted_points;

    for(int i = 0; i<m.vertices.size(); i += 3){
        vector3 point;
        point.set(m.vertices[i+0], m.vertices[i+1], m.vertices[i+2]);
        point.add(m.pos);
        vector2 new_point = convert_3d_to_2d(point, 40);
        converted_points.push_back(new_point);
    }

    //system("pause");
    std::string colors = "#@!SAOLK";
    for(int i = 0; i<m.triangles.size(); i+=3){
        //std::cout<<"index 1 triangulo:"<<m.triangles[i+0]<<std::endl;
        //std::cout<<"index 2 triangulo:"<<m.triangles[i+1]<<std::endl;
        //std::cout<<"index 3 triangulo:"<<m.triangles[i+2]<<std::endl;
        //system("pause");
        char color = '#';//colors[rand()%colors.length()-1];
        draw_triangle(s, converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1], color);
        //std::cout<<"DESENHEI UM TRIANGULO!!!!!!!"<<std::endl;
    }

    std::cout<<"TERMINEI DE RENDERIZAR A MESH!!!!!!!"<<std::endl;
    //system("pause");
}
