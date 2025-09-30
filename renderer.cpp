#include <iostream>
#include <vector>
#include "vector.h"
#include "screen.h"
#include "renderer.h"
#include <math.h>
#include "mesh.h"

//fazer funcao local to world
//fazer funcao world to screen

vector3 local_to_world(vector3 point, vector3 model_position, vector3 model_rotation, vector3 scale){
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

    vector3 final_point;

    //rotated by x axix
    final_point.x = point.x*rotationxx.x + point.y*rotationxx.y + point.z*rotationxx.z;
    final_point.y = point.x*rotationxy.x + point.y*rotationxy.y + point.z*rotationxy.z;
    final_point.z = point.x*rotationxz.x + point.y*rotationxz.y + point.z*rotationxz.z;

    point = final_point;
    //rotated by y axix
    final_point.x = point.x*rotationyx.x + point.y*rotationyx.y + point.z*rotationyx.z;
    final_point.y = point.x*rotationyy.x + point.y*rotationyy.y + point.z*rotationyy.z;
    final_point.z = point.x*rotationyz.x + point.y*rotationyz.y + point.z*rotationyz.z;

    point = final_point;
    //rotated by z axix
    final_point.x = point.x*rotationzx.x + point.y*rotationzx.y + point.z*rotationzx.z;
    final_point.y = point.x*rotationzy.x + point.y*rotationzy.y + point.z*rotationzy.z;
    final_point.z = point.x*rotationzz.x + point.y*rotationzz.y + point.z*rotationzz.z;

    //scale
    final_point.x *= scale.x;
    final_point.y *= scale.y;
    final_point.z *= scale.z;

    //position
    final_point.add(model_position);

    return final_point;
}

vector2 world_to_screen(vector3 point, float fov, int screen_width, int screen_height){
    vector2 new_point;
    new_point.x = screen_width  / 2 + (fov * point.x) / (fov + point.z);
    new_point.y = screen_height / 2 + (fov * point.y) / (fov + point.z);

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

void draw_triangle(screen* s, vector2 a, vector2 b, vector2 c, char color, vector3 zvlalues){
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
            if(!s->is_on_screen(j, i)) continue;
            vector2 point;
            point.set(j, i);
            vector3 current_weight;
            if(is_point_on_triangle(point, tri, &current_weight)){

                float depth = current_weight.dot(zvlalues);
                if(depth > s->depth_data[i][j]) continue;

                s->depth_data[i][j] = depth;
                //std::cout<<"depth:"<<depth<<std::endl;
                s->draw_pixel(j, i, color);
            }
        }
    }
    //std::cout<<"depois de desenhar triangulo"<<std::endl;
}

bool is_triangle_ccw(vector2 a, vector2 b, vector2 c){
//(x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2)) results in a positive value, the triangle is in counterclockwise order
    return ((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) > 0);
}

vector3 triangle_normal(vector3 a, vector3 b, vector3 c){
    vector3 v1 = b;
    v1.sub(a);
    vector3 v2 = c;
    v2.sub(a);
    vector3 normal;
    //cross product
    normal.x = (v1.y * v2.z) - (v1.z * v2.y);
    normal.y = (v1.z * v2.x) - (v1.x * v2.z);
    normal.z = (v1.x * v2.y) - (v1.y * v2.x);

    //normalize
    float magnitude = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    //std::cout<<"magnitude:"<<magnitude<<std::endl;
    //std::cout<<"Z:"<<normal.z<<std::endl;
    if(magnitude > 0){
        normal.x /= magnitude;
        normal.y /= magnitude;
        normal.z /= magnitude;
    }
    //std::cout<<"olha so to calculando o nomral:"<<std::endl;
    //normal.print();
    //std::cout<<"Z depois:"<<normal.z<<std::endl<<std::endl;
    return normal;
}

float range(float x1, float y1, float x2, float y2, float x){
    return (y1-x1) * ((x-x2)/(y2-x2)) + x1;
}

void render_mesh(screen* s, mesh m){
    std::vector<vector2> converted_points;
    std::vector<vector3> world_points;
    std::vector<vector3> normals;
    std::vector<float> zvalues_list;

    for(int i = 0; i<m.vertices.size(); i += 3){
        vector3 point;
        point.set(m.vertices[i+0], m.vertices[i+1], m.vertices[i+2]);
        point = local_to_world(point, m.pos, m.rotation, m.scale);
        world_points.push_back(point);
        zvalues_list.push_back(point.z);

        vector2 new_point = world_to_screen(point, 120, s->screen_width, s->screen_height);

        //normals.push_back(new_normal);
        converted_points.push_back(new_point);
    }

    for(int i = 0; i < world_points.size(); i += 3){
        //vector3 a = ;
        //std::cout<<"o normal q to coisando eh:"<<std::endl;
        //a.print();
        //normals.push_back(a);
    }

    //draw points
    /*
    for(int i = 0; i<m.triangles.size(); i++){
        char color = '#';
        s->draw_pixel(converted_points[i].x, converted_points[i].y, color);
    }*/

    //draw triangles:

    std::string colors = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    //"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:.\"^'.";
    //"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:.\"^'.";
    //".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    int color_count = -1;
    for(int i = 0; i<m.triangles.size(); i+=3){
        color_count++;
        if(color_count > colors.length()-1) color_count = 0;
        //check if the triangle is counter clockwise
        if(is_triangle_ccw(converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1]))
            continue;
        vector3 zvalues;
        zvalues.set(zvalues_list[m.triangles[i+0]-1], zvalues_list[m.triangles[i+1]-1], zvalues_list[m.triangles[i+2]-1]);
        //dot product with the normals
        vector3 light_direction;
        light_direction.set(0, -1, 0);

        float dot = triangle_normal(world_points[m.triangles[i+0]-1], world_points[m.triangles[i+1]-1], world_points[m.triangles[i+2]-1]).dot(light_direction);

        if(dot < -1 || dot > 1) continue;
        if(dot < 0) dot = 0;

        char color = colors[int(range(0, colors.length(), 0, 1, dot))];
        draw_triangle(s, converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1], color, zvalues);
    }
    std::cout<<"TERMINEI DE RENDERIZAR A MESH!!!!!!!"<<std::endl;
    //system("pause");
}
