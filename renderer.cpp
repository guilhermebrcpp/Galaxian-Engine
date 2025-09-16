#include <iostream>
#include <vector>
#include "vector.h"
#include "screen.h"
#include "renderer.h"
#include <math.h>
#include "mesh.h"

vector2 convert_3d_to_2d(vector3 point, vector3 model_position, vector3 model_rotation, int screen_width, int screen_height, float fov){
    //x rotation
    vector3 rotationxx; rotationxx.set(1, 0, 0);
    vector3 rotationxy; rotationxy.set(0, cos(model_rotation.x), -sin(model_rotation.x));
    vector3 rotationxz; rotationxz.set(0, sin(model_rotation.x), cos(model_rotation.x));
    //y rotation
    vector3 rotationyx; rotationyx.set(cos(model_rotation.y), 0, sin(model_rotation.y));
    vector3 rotationyy; rotationyy.set(0, 1, 0);
    vector3 rotationyz; rotationyz.set(-sin(model_rotation.y), 0, cos(model_rotation.y));
    //z rotation
    vector3 rotationzx; rotationzx.set(cos(model_rotation.z),-sin(model_rotation.z), 0);
    vector3 rotationzy; rotationzy.set(sin(model_rotation.z), cos(model_rotation.z), 0);
    vector3 rotationzz; rotationzz.set(0, 0, 1);

    vector3 rotated_point;

    //rotated by x axix
    rotated_point.x = point.x*rotationxx.x + point.y*rotationxx.y + point.z*rotationxx.z;
    rotated_point.y = point.x*rotationxy.x + point.y*rotationxy.y + point.z*rotationxy.z;
    rotated_point.z = point.x*rotationxz.x + point.y*rotationxz.y + point.z*rotationxz.z;

    vector3 rotated_pointx = rotated_point;
    //rotated by y axix
    rotated_point.x = rotated_pointx.x*rotationyx.x + rotated_pointx.y*rotationyx.y + rotated_pointx.z*rotationyx.z;
    rotated_point.y = rotated_pointx.x*rotationyy.x + rotated_pointx.y*rotationyy.y + rotated_pointx.z*rotationyy.z;
    rotated_point.z = rotated_pointx.x*rotationyz.x + rotated_pointx.y*rotationyz.y + rotated_pointx.z*rotationyz.z;

    vector3 rotated_pointy = rotated_point;
    //rotated by z axix
    rotated_point.x = rotated_pointy.x*rotationzx.x + rotated_pointy.y*rotationzx.y + rotated_pointy.z*rotationzx.z;
    rotated_point.y = rotated_pointy.x*rotationzy.x + rotated_pointy.y*rotationzy.y + rotated_pointy.z*rotationzy.z;
    rotated_point.z = rotated_pointy.x*rotationzz.x + rotated_pointy.y*rotationzz.y + rotated_pointy.z*rotationzz.z;

    //position
    rotated_point.add(model_position);

    vector2 new_point;
    new_point.x = screen_width  / 2 + (fov * rotated_point.x) / (fov + rotated_point.z);
    new_point.y = screen_height / 2 + (fov * rotated_point.y) / (fov + rotated_point.z);

    return new_point;
}

float triangle_area(vector2 a, vector2 b, vector2 c){
//(1/2) (( x1(y2 − y3) + x2(y3 − y1) + x3(y1 − y2)))
    return (abs((a.x*(b.y - c.y)) + (b.x*(c.y - a.y)) + (c.x*(a.y - b.y))))/2;
}

bool is_point_on_triangle(vector2 p, vector2 tri[], vector3 *weights){
    float tri_area = triangle_area(tri[0], tri[1], tri[2]);

    float area1 = triangle_area(tri[0], tri[1], p);
    float area2 = triangle_area(tri[1], tri[2], p);
    float area3 = triangle_area(tri[2], tri[0], p);

    float inv_area_sum = 1 / (area1 + area2 + area3);
    float weightA = area1 * inv_area_sum;
    float weightB = area2 * inv_area_sum;
    float weightC = area3 * inv_area_sum;
    weights->set(weightA, weightB, weightC);

    return ((area1 + area2 + area3)-10 < tri_area) && tri_area > 0;
}

void draw_triangle(screen* s, vector2 a, vector2 b, vector2 c, char color){
    int bounding_box_x_min = std::min(std::min(a.x, b.x), c.x);
    int bounding_box_x_max = std::max(std::max(a.x, b.x), c.x);
    int bounding_box_y_min = std::min(std::min(a.y, b.y), c.y);
    int bounding_box_y_max = std::max(std::max(a.y, b.y), c.y);

    vector2 tri[3] = {a, b, c};
    /*
    std::cout<<"antes de desenhar triangulo"<<std::endl;
    std::cout<<"minX"<<bounding_box_x_min<<"   maxX"<<bounding_box_x_max<<std::endl;
    std::cout<<"minY"<<bounding_box_y_min<<"   maxY"<<bounding_box_y_max<<std::endl;
    */
    for(int i = bounding_box_y_min; i < bounding_box_y_max; i++){
        for(int j = bounding_box_x_min; j < bounding_box_x_max; j++){
            vector2 point;
            point.set(j, i);
            vector3 current_weight;
            if(is_point_on_triangle(point, tri, &current_weight)){
                s->draw_pixel(j, i, color);
                //current_weight
                //s->depth_data[i][j] =
            }
        }
    }
    //std::cout<<"depois de desenhar triangulo"<<std::endl;
}

bool is_triangle_ccw(vector2 a, vector2 b, vector2 c){
//(x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2)) results in a positive value, the triangle is in counterclockwise order
    return ((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) > 0);
}

void render_mesh(screen* s, mesh m){
    std::vector<vector2> converted_points;

    for(int i = 0; i<m.vertices.size(); i += 3){
        vector3 point;
        point.set(m.vertices[i+0], m.vertices[i+1], m.vertices[i+2]);
        vector2 new_point = convert_3d_to_2d(point, m.pos, m.rotation, s->screen_width, s->screen_height, 90);
        converted_points.push_back(new_point);
    }

    //draw points
    /*
    for(int i = 0; i<m.triangles.size(); i++){
        char color = '#';
        s->draw_pixel(converted_points[i].x, converted_points[i].y, color);
    }*/

    //draw triangles:
    std::string colors = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    int color_count = -1;
    for(int i = 0; i<m.triangles.size(); i+=3){
        color_count++;
        if(color_count > colors.length()-1) color_count = 0;
        //check if the triangle is counter clockwise
        if(is_triangle_ccw(converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1]))
            continue;

        char color = colors[color_count];
        draw_triangle(s, converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1], color);
        //std::cout<<"DESENHEI UM TRIANGULO!!!!!!!"<<std::endl;

    }

    std::cout<<"TERMINEI DE RENDERIZAR A MESH!!!!!!!"<<std::endl;
    //system("pause");
}
