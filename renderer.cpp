#include <iostream>
#include <vector>
#include "vector.h"
#include "screen.h"
#include "renderer.h"
#include <math.h>
#include "mesh.h"
#include "texture.h"
#include "inputHandler.h"

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

float range(float x1, float y1, float x2, float y2, float x){
    return (y1-x1) * ((x-x2)/(y2-x2)) + x1;
}


void draw_triangle(screen* s, vector2 a, vector2 b, vector2 c, vector2 tex_pos1, vector2 tex_pos2, vector2 tex_pos3, char color, vector3 zvalues, texture *tex, bool has_texture){
    int bounding_box_x_min = std::min(std::min(std::ceil(a.x), std::ceil(b.x)), std::ceil(c.x));
    int bounding_box_x_max = std::max(std::max(std::ceil(a.x), std::ceil(b.x)), std::ceil(c.x));
    int bounding_box_y_min = std::min(std::min(std::ceil(a.y), std::ceil(b.y)), std::ceil(c.y));
    int bounding_box_y_max = std::max(std::max(std::ceil(a.y), std::ceil(b.y)), std::ceil(c.y));

    //make sure its not outside the screen

    bounding_box_x_min = std::min(s->screen_width-1, std::max(0, bounding_box_x_min));
    bounding_box_x_max = std::min(s->screen_width-1, std::max(0, bounding_box_x_max));
    bounding_box_y_min = std::min(s->screen_height-1, std::max(0, bounding_box_y_min));
    bounding_box_y_max = std::min(s->screen_height-1, std::max(0, bounding_box_y_max));

    vector2 tri[3] = {a, b, c};

    for(int i = bounding_box_y_min; i < bounding_box_y_max; i++){
        for(int j = bounding_box_x_min; j < bounding_box_x_max; j++){
            if(!s->is_on_screen(j, i)) continue;
            vector2 point;
            point.set(j, i);
            vector3 current_weight;

            if(is_point_on_triangle(point, tri, &current_weight)){

                float invZ = current_weight.x * (1 / zvalues.x) +
                current_weight.y * (1 / zvalues.y) +
                current_weight.z * (1 / zvalues.z);

                float depth = 1 / invZ;//current_weight.dot(zvalues);

                if(depth > s->depth_data[i][j]) continue;

                //calculate the uv coordinate to get the current texture pixel
                if(has_texture){
                    vector2 temp_tex_coord; temp_tex_coord.set(0, 0);

                    temp_tex_coord.x += (tex_pos1.x / zvalues.x) * current_weight.x;
                    temp_tex_coord.y += (tex_pos1.y / zvalues.x) * current_weight.x;

                    temp_tex_coord.x += (tex_pos2.x / zvalues.y) * current_weight.y;
                    temp_tex_coord.y += (tex_pos2.y / zvalues.y) * current_weight.y;

                    temp_tex_coord.x += (tex_pos3.x / zvalues.z) * current_weight.z;
                    temp_tex_coord.y += (tex_pos3.y / zvalues.z) * current_weight.z;

                    temp_tex_coord.x /= invZ;
                    temp_tex_coord.y /= invZ;

                    //float min_y_uv = std::min(0.0, double(std::floor(temp_tex_coord.y)));
                    //float max_y_uv = std::max(1.0, double(std::ceil(temp_tex_coord.y)));
                    //float min_x_uv = std::min(0.0, double(std::floor(temp_tex_coord.x)));
                    //float max_x_uv = std::max(1.0, double(std::ceil(temp_tex_coord.x)));

                    temp_tex_coord.x -= std::floor(temp_tex_coord.x);
                    temp_tex_coord.y -= std::floor(temp_tex_coord.y);

                    int final_texture_x = floor(range(0, tex->get_width()-1, 0, 1, temp_tex_coord.x));
                    int final_texture_y = floor(range(0, tex->get_height()-1, 1, 0, temp_tex_coord.y));

                    final_texture_x = std::min(final_texture_x, tex->get_width()-1);
                    final_texture_y = std::min(final_texture_y, tex->get_height()-1);

                    final_texture_x = std::max(final_texture_x, 0);
                    final_texture_y = std::max(final_texture_y, 0);

                    //std::cout<<"final_texture X:"<<final_texture_x<<std::endl;
                    //std::cout<<"final_texture Y:"<<final_texture_y<<std::endl;
                    //system("pause");
                    color = tex->data[final_texture_y][final_texture_x];
                }



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

int cu = 18;


void render_mesh(screen* s, mesh m, camera cam){
    //draw triangles:

    std::string colors = " .:-=+*#%@";

    for(int current_sub_mesh = 0; current_sub_mesh < m.triangles.size(); current_sub_mesh++){

        //std::cout<<"\nsubmesh:"<<current_sub_mesh<<std::endl;
        //system("pause");

        //if(current_sub_mesh != cu) continue;
        for(int i = 0; i < m.triangles[current_sub_mesh].size(); i+=3){

            vector3 point1; point1.set(m.vertices[((m.triangles[current_sub_mesh][i+0]-1)*3)+0],
                                       m.vertices[((m.triangles[current_sub_mesh][i+0]-1)*3)+1],
                                       m.vertices[((m.triangles[current_sub_mesh][i+0]-1)*3)+2]);

            vector3 point2; point2.set(m.vertices[((m.triangles[current_sub_mesh][i+1]-1)*3)+0],
                                       m.vertices[((m.triangles[current_sub_mesh][i+1]-1)*3)+1],
                                       m.vertices[((m.triangles[current_sub_mesh][i+1]-1)*3)+2]);

            vector3 point3; point3.set(m.vertices[((m.triangles[current_sub_mesh][i+2]-1)*3)+0],
                                       m.vertices[((m.triangles[current_sub_mesh][i+2]-1)*3)+1],
                                       m.vertices[((m.triangles[current_sub_mesh][i+2]-1)*3)+2]);


            vector3 world_points[3] = {
                to_view_space(local_to_world(point1, m.pos, m.rotation, m.scale), cam.pos, cam.rotation),
                to_view_space(local_to_world(point2, m.pos, m.rotation, m.scale), cam.pos, cam.rotation),
                to_view_space(local_to_world(point3, m.pos, m.rotation, m.scale), cam.pos, cam.rotation)
            };

            vector2 converted_points[3] = {
                world_to_screen(world_points[0], cam.fov, s->screen_width, s->screen_height),
                world_to_screen(world_points[1], cam.fov, s->screen_width, s->screen_height),
                world_to_screen(world_points[2], cam.fov, s->screen_width, s->screen_height)
            };

            /*
            std::cout<<"ponto 1: x:"<<converted_points[0].x<<" y:"<<converted_points[0].y<<std::endl;
            std::cout<<"ponto 2: x:"<<converted_points[1].x<<" y:"<<converted_points[1].y<<std::endl;
            std::cout<<"ponto 3: x:"<<converted_points[2].x<<" y:"<<converted_points[2].y<<std::endl<<std::endl;
            */
            //system("pause");
            int current_vertices[3] = {m.triangles[current_sub_mesh][i+0]-1, m.triangles[current_sub_mesh][i+1]-1, m.triangles[current_sub_mesh][i+2]-1};

            //std::cout<<"\ntamanho:"<<m.triangles[current_sub_mesh].size()<<"\nsubmesh:"<<current_sub_mesh<<"\ntriangulo:"<<i<<"\ncurrent1:"<<current_vertices[0]<<"\n"<<"current2:"<<current_vertices[1]<<"\ncurrent3:"<<current_vertices[2]<<std::endl;

            //check if the triangle is counter clockwise
            if(!is_triangle_ccw(converted_points[0],
                                converted_points[1],
                                converted_points[2]))
                continue;

            vector3 zvalues;
            zvalues.set(world_points[0].z, world_points[1].z, world_points[2].z);
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

            vector3 current_normal = triangle_normal(world_points[0], world_points[1], world_points[2]);
            float dot = current_normal.dot(light_direction);

            if(dot < -1 || dot > 1) continue;
            if(dot < 0) dot = 0;
            if(dot > 1) dot = 1;
            float light_level = std::min(dot+0.23, 1.0);

            char color = colors[int(range(0, colors.length()-1, 0, 1, light_level))];
            /*
            for(int y = 0; y < m.materials[m.get_current_material(current_sub_mesh)].albedo_texture.get_height(); y ++){
                for(int x = 0; x < m.materials[m.get_current_material(current_sub_mesh)].albedo_texture.get_width(); x ++){
                    std::cout<<m.materials[m.get_current_material(current_sub_mesh)].albedo_texture.data[y][x];
                }
                std::cout<<"\n";
            }*/

            draw_triangle(s, converted_points[0],
                             converted_points[1],
                             converted_points[2],
                             m.vertex_texture[m.vertex_texture_indices[current_sub_mesh][i+0]-1],
                             m.vertex_texture[m.vertex_texture_indices[current_sub_mesh][i+1]-1],
                             m.vertex_texture[m.vertex_texture_indices[current_sub_mesh][i+2]-1],
                             color, zvalues, &m.materials[m.get_current_material(current_sub_mesh)].albedo_texture, m.have_texture());
        }
    }
    std::cout<<"TERMINEI DE RENDERIZAR A MESH!!!!!!!"<<std::endl;
    //system("pause");
}
