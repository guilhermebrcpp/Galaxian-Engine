#include <iostream>
#include <vector>
#include "vector.h"
#include "screen.h"
#include "renderer.h"
#include <math.h>
#include "mesh.h"

vector3 local_to_world(vector3 point, vector3 model_position, vector3 model_rotation, vector3 scale){
    vector3 final_point = point;
    //rotation
    final_point = rotated_by_x(final_point, model_rotation.x);
    final_point = rotated_by_y(final_point, model_rotation.y);
    final_point = rotated_by_z(final_point, model_rotation.z);

    //scale
    final_point.x *= scale.x;
    final_point.y *= scale.y;
    final_point.z *= scale.z;

    //position
    final_point.add(model_position);

    return final_point;
}

vector3 to_view_space(vector3 point, vector3 camera_position, vector3 camera_rotation){
    //position
    point.sub(camera_position);
    vector3 final_point = point;
    //rotation
    final_point = rotated_by_y(final_point, -camera_rotation.y);
    final_point = rotated_by_x(final_point, -camera_rotation.x);

    return final_point;
}

vector2 world_to_screen(vector3 point, float fov, int screen_width, int screen_height){
    vector2 new_point;

    new_point.x = screen_width  / 2 + (point.x * fov) / point.z;
    new_point.y = screen_height / 2 - (point.y * fov) / point.z;

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
    float weightA = area2 * inv_area_sum;
    float weightB = area3 * inv_area_sum;
    float weightC = area1 * inv_area_sum;
    weights->set(weightA, weightB, weightC);

    vector2 v1 = tri[0];
    v1.sub(tri[1]);
    v1.set(v1.y, -v1.x);
    vector2 to_p;
    to_p.set(p.x, p.y);
    to_p.sub(tri[0]);

    float a = to_p.dot(v1);

    vector2 v2 = tri[1];
    v2.sub(tri[2]);
    v2.set(v2.y, -v2.x);
    to_p.set(p.x, p.y);
    to_p.sub(tri[1]);

    float b = to_p.dot(v2);

    vector2 v3 = tri[2];
    v3.sub(tri[0]);
    v3.set(v3.y, -v3.x);
    to_p.set(p.x, p.y);
    to_p.sub(tri[2]);

    float c = to_p.dot(v3);

    return (a >= 0 && b >= 0 && c >= 0) && tri_area > 0;
}

void draw_triangle(screen* s, vector2 a, vector2 b, vector2 c, char color, vector3 zvalues){
    int bounding_box_x_min = std::min(std::min(std::ceil(a.x), std::ceil(b.x)), std::ceil(c.x));
    int bounding_box_x_max = std::max(std::max(std::ceil(a.x), std::ceil(b.x)), std::ceil(c.x));
    int bounding_box_y_min = std::min(std::min(std::ceil(a.y), std::ceil(b.y)), std::ceil(c.y));
    int bounding_box_y_max = std::max(std::max(std::ceil(a.y), std::ceil(b.y)), std::ceil(c.y));

    //make sure its not outside the screen

    bounding_box_x_min = std::min(s->screen_width, std::max(0, bounding_box_x_min));
    bounding_box_x_max = std::min(s->screen_width, std::max(0, bounding_box_x_max));
    bounding_box_y_min = std::min(s->screen_height, std::max(0, bounding_box_y_min));
    bounding_box_y_max = std::min(s->screen_height, std::max(0, bounding_box_y_max));

    vector2 tri[3] = {a, b, c};

    for(int i = bounding_box_y_min; i < bounding_box_y_max; i++){
        for(int j = bounding_box_x_min; j < bounding_box_x_max; j++){
            if(!s->is_on_screen(j, i)) continue;
            vector2 point;
            point.set(j, i);
            vector3 current_weight;
            if(is_point_on_triangle(point, tri, &current_weight)){

                //float invZ = current_weight.x * (1.0f / zvalues.x) +
                //current_weight.y * (1.0f / zvalues.y) +
                //current_weight.z * (1.0f / zvalues.z);


                float depth = current_weight.dot(zvalues);

                if(depth > s->depth_data[i][j]) continue;

                s->depth_data[i][j] = depth;
                s->draw_pixel(j, i, color);
            }
        }
    }
    //std::cout<<"depois de desenhar triangulo"<<std::endl;
}

bool is_triangle_ccw(vector2 a, vector2 b, vector2 c){
//if (x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2)) results in a positive value, the triangle is in counterclockwise order
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
    if(magnitude > 0){
        normal.x /= magnitude;
        normal.y /= magnitude;
        normal.z /= magnitude;
    }
    return normal;
}

float range(float x1, float y1, float x2, float y2, float x){
    return (y1-x1) * ((x-x2)/(y2-x2)) + x1;
}

void render_mesh(screen* s, mesh m, camera cam){
    std::vector<vector2> converted_points;
    std::vector<vector3> world_points;
    std::vector<float> zvalues_list;

    for(int i = 0; i<m.vertices.size(); i += 3){
        vector3 point;
        point.set(m.vertices[i+0], m.vertices[i+1], m.vertices[i+2]);
        point = local_to_world(point, m.pos, m.rotation, m.scale);
        point = to_view_space(point, cam.pos, cam.rotation);

        world_points.push_back(point);
        zvalues_list.push_back(point.z);

        vector2 new_point = world_to_screen(point, cam.fov, s->screen_width, s->screen_height);

        converted_points.push_back(new_point);
    }

    //draw triangles:

    std::string colors = ".'`^\",:;Il!i~+-?][}{1)(|\\/tfXYUJCLQ0OZ#MW&8%B@$";
    //"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:.\"^'.";
    //"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:.\"^'.";
    //".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    int current_color = 0;
    for(int i = 0; i<m.triangles.size(); i+=3){
        //check if the triangle is counter clockwise
        if(!is_triangle_ccw(converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1]))
            continue;
        vector3 zvalues;
        zvalues.set(zvalues_list[m.triangles[i+0]-1], zvalues_list[m.triangles[i+1]-1], zvalues_list[m.triangles[i+2]-1]);
        //check if the z values are negative (behing camera)
        float minzvaluerendered = 0.1;
        if(zvalues.x <= minzvaluerendered || zvalues.y <= minzvaluerendered || zvalues.z <= minzvaluerendered)
            continue;
        //dot product with the normals
        vector3 light_direction;
        light_direction.set(1, 1, 0);
        light_direction = light_direction.normalized();
        //inverse rotating the light by the camera angle to fix the light angle
        light_direction = rotated_by_y(light_direction, -cam.rotation.y);
        light_direction = rotated_by_x(light_direction, -cam.rotation.x);

        vector3 current_normal = triangle_normal(world_points[m.triangles[i+0]-1], world_points[m.triangles[i+1]-1], world_points[m.triangles[i+2]-1]);
        float dot = current_normal.dot(light_direction);

        if(dot < -1 || dot > 1) continue;
        if(dot < 0) dot = 0;
        if(dot > 1) dot = 1;
        float light_level = std::min(dot+0.23, 1.0);

        char color = colors[int(range(0, colors.length()-1, 0, 1, light_level))];
        //color = colors[current_color];
        current_color++;
        if(current_color >= colors.length()-1) current_color = 0;
        draw_triangle(s, converted_points[m.triangles[i+0]-1], converted_points[m.triangles[i+1]-1], converted_points[m.triangles[i+2]-1], color, zvalues);
    }
    std::cout<<"TERMINEI DE RENDERIZAR A MESH!!!!!!!"<<std::endl;
    //system("pause");
}
