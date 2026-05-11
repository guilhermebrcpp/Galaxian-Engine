#include <iostream>
#include <vector>
#include "vector.h"
#include "screen.h"
#include "renderer.h"
#include <math.h>
#include "mesh.h"
#include "texture.h"
#include <cstring>
#include <chrono>

//for SIMD AVX2
#include <immintrin.h>

inline vector3 local_to_world(vector3 point, vector3 model_position, vector3 model_rotation, vector3 scale){
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

inline vector3 to_view_space(vector3 point, vector3 camera_position, vector3 camera_rotation){
    //position
    point.sub(camera_position);
    vector3 final_point = point;
    //rotation
    final_point = rotated_by_y(final_point, -camera_rotation.y);
    final_point = rotated_by_x(final_point, -camera_rotation.x);

    return final_point;
}

inline vector2 world_to_screen(vector3 point, float fov, int screen_width, int screen_height){
    vector2 new_point;

    new_point.x = screen_width  / 2 + (point.x * fov) / point.z;
    new_point.y = screen_height / 2 - (point.y * fov) / point.z;

    return new_point;
}

float triangle_area(vector2 a, vector2 b, vector2 c){
//(1/2) (( x1(y2 − y3) + x2(y3 − y1) + x3(y1 − y2)))
    return (abs((a.x*(b.y - c.y)) + (b.x*(c.y - a.y)) + (c.x*(a.y - b.y))))/2;
}

bool is_point_on_triangle(float w0, float w1, float w2){
    return (w0 >= 0 && w1 >= 0 && w2 >= 0);
}

inline float range(float x1, float y1, float x2, float y2, float x){
    return (y1-x1) * ((x-x2)/(y2-x2)) + x1;
}

float cross(vector2 p1, vector2 p2){
	return (p1.x * p2.y - p2.x * p1.y);
}

inline float edge_function(vector2 p1, vector2 p2, vector2 po){
	vector2 v1; v1.set(p2.x - p1.x, p2.y - p1.y);
	vector2 v2; v2.set(po.x - p1.x, po.y - p1.y);

	return cross(v1, v2);
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

    float z1 = zvalues.x;
    float z2 = zvalues.y;
    float z3 = zvalues.z;

    float inverse_z1 = 1 / z1;
    float inverse_z2 = 1 / z2;
    float inverse_z3 = 1 / z3;

    vector2 point;
    vector3 current_weight;

    //pre calculate things hehehe ha
    vector2 p; p.set(bounding_box_x_min, bounding_box_y_min);
    vector2 p2; p2.set(p.x-1, p.y);
    float dw0_x = edge_function(a, b, p) - edge_function(a, b, p2);
    float dw1_x = edge_function(b, c, p) - edge_function(b, c, p2);
    float dw2_x = edge_function(c, a, p) - edge_function(c, a, p2);

    p2.set(p.x, p.y+1);
    float dw0_y = edge_function(a, b, p2) - edge_function(a, b, p);
    float dw1_y = edge_function(b, c, p2) - edge_function(b, c, p);
    float dw2_y = edge_function(c, a, p2) - edge_function(c, a, p);

    float w0_start_x = edge_function(a, b, p);
    float w1_start_x = edge_function(b, c, p);
    float w2_start_x = edge_function(c, a, p);

    //delta x positions
    __m256 _dw0_x = _mm256_set1_ps(dw0_x*8);
    __m256 _dw1_x = _mm256_set1_ps(dw1_x*8);
    __m256 _dw2_x = _mm256_set1_ps(dw2_x*8);

    //delta y positions
    __m256 _dw0_y = _mm256_set1_ps(dw0_y);
    __m256 _dw1_y = _mm256_set1_ps(dw1_y);
    __m256 _dw2_y = _mm256_set1_ps(dw2_y);

    __m256 _sequential = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);

    //start vector changed by y
    //calculate w0_start_x (w0_start_x = w0_start_x + dw0_x * n)
    __m256 _dw_x_temp = _mm256_set1_ps(dw0_x);
    __m256 _w0_start_x = _mm256_set1_ps(w0_start_x);
    _w0_start_x = _mm256_fmadd_ps(_sequential, _dw_x_temp, _w0_start_x);

    //calculate w1_start_x
    _dw_x_temp = _mm256_set1_ps(dw1_x);
    __m256 _w1_start_x = _mm256_set1_ps(w1_start_x);
    _w1_start_x = _mm256_fmadd_ps(_sequential, _dw_x_temp, _w1_start_x);

    //calculate w2_start_x
    _dw_x_temp = _mm256_set1_ps(dw2_x);
    __m256 _w2_start_x = _mm256_set1_ps(w2_start_x);
    _w2_start_x = _mm256_fmadd_ps(_sequential, _dw_x_temp, _w2_start_x);

    __m256 _zeroes = _mm256_set1_ps(0.0f);

    float current_triangle_area = edge_function(a, b, c);


    __m256 _minus_one = _mm256_set1_ps(-1.0f);
    __m256 _one = _mm256_set1_ps(1.0f);

    for(int i = bounding_box_y_min; i < bounding_box_y_max; i++){
        __m256 _w0 = _w0_start_x;
        __m256 _w1 = _w1_start_x;
        __m256 _w2 = _w2_start_x;
        int j;
        for(j = bounding_box_x_min; j <= bounding_box_x_max-8; j+=8){
            // w0 >= 0, w1 >= 0, w2 >= 0
            __m256 _mask1 = _mm256_cmp_ps(_w0, _zeroes, _CMP_GE_OQ);
        	__m256 _mask2 = _mm256_cmp_ps(_w1, _zeroes, _CMP_GE_OQ);
        	__m256 _mask3 = _mm256_cmp_ps(_w2, _zeroes, _CMP_GE_OQ);

            //using AND operation to join the bitmasks
            __m256 _maskf = _mm256_and_ps(_mm256_and_ps(_mask1, _mask2), _mask3);
            char bit_mask = _mm256_movemask_ps(_maskf);

            if(bit_mask == 0){
                _w0 = _mm256_add_ps(_w0, _dw0_x);
                _w1 = _mm256_add_ps(_w1, _dw1_x);
                _w2 = _mm256_add_ps(_w2, _dw2_x);
                continue;
            }

            //***************** calculate Z of the current pixel ****************
            __m256 _triangle_area = _mm256_set1_ps(current_triangle_area);
            __m256 _current_weight_x = _mm256_div_ps(_w1, _triangle_area);
            __m256 _current_weight_y = _mm256_div_ps(_w2, _triangle_area);
            __m256 _current_weight_z = _mm256_div_ps(_w0, _triangle_area);

            __m256 _inverse_zn = _mm256_set1_ps(inverse_z1);
            __m256 _invZ = _mm256_mul_ps(_current_weight_x, _inverse_zn);

            _inverse_zn = _mm256_set1_ps(inverse_z2);
            _invZ = _mm256_add_ps(_invZ, _mm256_mul_ps(_current_weight_y, _inverse_zn));

            _inverse_zn = _mm256_set1_ps(inverse_z3);
            _invZ = _mm256_add_ps(_invZ, _mm256_mul_ps(_current_weight_z, _inverse_zn));

            __m256 _depth;// = _mm256_set1_ps(1.0f);
            _depth = _mm256_rcp_ps(_invZ);//_mm256_div_ps(_depth, _invZ);

            //************* calculate texture coordinates **************
            int final_textures_x[8];
            int final_textures_y[8];
            if(has_texture == true){
                __m256 _temp_tex_coord_x = _mm256_set1_ps(0.0f);
                __m256 _temp_tex_coord_y = _mm256_set1_ps(0.0f);

                //**********calculate weights for z1
                //(tex_pos1.x / z1) * current_weight.x;
                //(tex_pos1.y / z1) * current_weight.x;
                __m256 _tex_pos = _mm256_set1_ps(tex_pos1.x);
                __m256 _z = _mm256_set1_ps(z1);
                _temp_tex_coord_x = _mm256_add_ps(_temp_tex_coord_x, _mm256_mul_ps(_current_weight_x, _mm256_div_ps(_tex_pos, _z)));

                _tex_pos = _mm256_set1_ps(tex_pos1.y);
                _temp_tex_coord_y = _mm256_add_ps(_temp_tex_coord_y, _mm256_mul_ps(_current_weight_x, _mm256_div_ps(_tex_pos, _z)));

                //**********calculate weights for z2
                //(tex_pos2.x / z2) * current_weight.y;
                //(tex_pos2.y / z2) * current_weight.y;
                _tex_pos = _mm256_set1_ps(tex_pos2.x);
                _z = _mm256_set1_ps(z2);
                _temp_tex_coord_x = _mm256_add_ps(_temp_tex_coord_x, _mm256_mul_ps(_current_weight_y, _mm256_div_ps(_tex_pos, _z)));

                _tex_pos = _mm256_set1_ps(tex_pos2.y);
                _temp_tex_coord_y = _mm256_add_ps(_temp_tex_coord_y, _mm256_mul_ps(_current_weight_y, _mm256_div_ps(_tex_pos, _z)));


                //**********calculate weights for z3
                //(tex_pos3.x / z3) * current_weight.z;
                //(tex_pos3.y / z3) * current_weight.z;
                _tex_pos = _mm256_set1_ps(tex_pos3.x);
                _z = _mm256_set1_ps(z3);
                _temp_tex_coord_x = _mm256_add_ps(_temp_tex_coord_x, _mm256_mul_ps(_current_weight_z, _mm256_div_ps(_tex_pos, _z)));

                _tex_pos = _mm256_set1_ps(tex_pos3.y);
                _temp_tex_coord_y = _mm256_add_ps(_temp_tex_coord_y, _mm256_mul_ps(_current_weight_z, _mm256_div_ps(_tex_pos, _z)));

                //divide by 1/depth
                //__m256 ones = _mm256_set1_ps(1.0f);

                _temp_tex_coord_x = _mm256_div_ps(_temp_tex_coord_x, _mm256_rcp_ps(_depth));
                _temp_tex_coord_y = _mm256_div_ps(_temp_tex_coord_y, _mm256_rcp_ps(_depth));

                //temp_tex_coord.x -= std::floor(temp_tex_coord.x);
                _temp_tex_coord_x = _mm256_sub_ps(_temp_tex_coord_x, _mm256_floor_ps(_temp_tex_coord_x));
                _temp_tex_coord_y = _mm256_sub_ps(_temp_tex_coord_y, _mm256_floor_ps(_temp_tex_coord_y));

                //int final_texture_x = (tex->get_width()-1) * temp_tex_coord.x;
                //int final_texture_y = (tex->get_height()-1) * (-temp_tex_coord.y + 1);
                __m256 _temp_tex_width = _mm256_set1_ps(tex->get_width()-1);
                _temp_tex_coord_x = _mm256_mul_ps(_temp_tex_coord_x, _temp_tex_width);


                __m256 _temp_tex_height = _mm256_set1_ps(tex->get_height()-1);

                _temp_tex_coord_y = _mm256_fmadd_ps(_temp_tex_coord_y, _minus_one, _one);
                _temp_tex_coord_y = _mm256_mul_ps(_temp_tex_coord_y, _temp_tex_height);

                //set into integer registers
                __m256i _final_tex_coord_x = _mm256_cvttps_epi32(_temp_tex_coord_x);
                __m256i _final_tex_coord_y = _mm256_cvttps_epi32(_temp_tex_coord_y);

                _mm256_store_si256((__m256i*)final_textures_x, _final_tex_coord_x);
                _mm256_store_si256((__m256i*)final_textures_y, _final_tex_coord_y);
            }

            float depths[8];        _mm256_store_ps(depths, _depth);

            //checking the bitmask to see which pixels are inside the triangle
            for(int i2 = 0; i2 < 8; i2++){
                if((bit_mask & (1<<i2)) > 0){
                    //check depth data
                    if(depths[i2] > s->get_depth_data(i, j+i2)) continue;

                    //calculate the uv coordinate to get the current texture pixel
                    if(has_texture){
                        //printf("vou pintar heheh: x: %d, y: %d\n", final_textures_y[i2], final_textures_x[i2]);

                        int final_texture_x0 = final_textures_x[i2];
                        int final_texture_y0 = final_textures_y[i2];

                        //int final_texture_x0 = std::min(final_textures_x[i2], tex->get_width()-1);
                        //int final_texture_y0 = std::min(final_textures_y[i2], tex->get_height()-1);

                        //final_texture_x0 = std::max(final_texture_x0, 0);
                        //final_texture_y0 = std::max(final_texture_y0, 0);


                        color = tex->data[tex->get_width() * final_texture_y0 + final_texture_x0];//final_texture_y][final_texture_x];
                    }

                    //_mm256_maskload_ps s->
                    s->set_depth_data(i, j+i2, depths[i2]);
                    s->draw_pixel(j+i2, i, color);
                }
            }
            _w0 = _mm256_add_ps(_w0, _dw0_x);
            _w1 = _mm256_add_ps(_w1, _dw1_x);
            _w2 = _mm256_add_ps(_w2, _dw2_x);
        }

        float new_iw0 = w0_start_x + (dw0_y * (i - bounding_box_y_min));
        float new_iw1 = w1_start_x + (dw1_y * (i - bounding_box_y_min));
        float new_iw2 = w2_start_x + (dw2_y * (i - bounding_box_y_min));

        float new_w0 = new_iw0 + (dw0_x * (j - bounding_box_x_min));
        float new_w1 = new_iw1 + (dw1_x * (j - bounding_box_x_min));
        float new_w2 = new_iw2 + (dw2_x * (j - bounding_box_x_min));

        for(; j < bounding_box_x_max; j++){
            //printf("sobrou!!\n");
            if(new_w0 >= 0 && new_w1 >= 0 && new_w2 >= 0){
                point.set(j, i);

                    current_weight.x = edge_function(b, c, point) / current_triangle_area;
                    current_weight.y = edge_function(c, a, point) / current_triangle_area;
                    current_weight.z = edge_function(a, b, point) / current_triangle_area;


                    float invZ = current_weight.x * (inverse_z1) +
                    current_weight.y * (inverse_z2) +
                    current_weight.z * (inverse_z3);

                    float depth = 1 / invZ;//current_weight.dot(zvalues);

                    if(depth > s->get_depth_data(i, j)) continue;

                    //calculate the uv coordinate to get the current texture pixel
                    if(has_texture){
                        vector2 temp_tex_coord; temp_tex_coord.set(0, 0);

                        temp_tex_coord.x += (tex_pos1.x / z1) * current_weight.x;
                        temp_tex_coord.y += (tex_pos1.y / z1) * current_weight.x;

                        temp_tex_coord.x += (tex_pos2.x / z2) * current_weight.y;
                        temp_tex_coord.y += (tex_pos2.y / z2) * current_weight.y;

                        temp_tex_coord.x += (tex_pos3.x / z3) * current_weight.z;
                        temp_tex_coord.y += (tex_pos3.y / z3) * current_weight.z;

                        temp_tex_coord.x /= invZ;
                        temp_tex_coord.y /= invZ;

                        temp_tex_coord.x -= std::floor(temp_tex_coord.x);
                        temp_tex_coord.y -= std::floor(temp_tex_coord.y);

                        int final_texture_x = (tex->get_width()-1) * temp_tex_coord.x;
                        int final_texture_y = (tex->get_height()-1) * (-temp_tex_coord.y + 1);

                        final_texture_x = std::min(final_texture_x, tex->get_width()-1);
                        final_texture_y = std::min(final_texture_y, tex->get_height()-1);

                        final_texture_x = std::max(final_texture_x, 0);
                        final_texture_y = std::max(final_texture_y, 0);

                        color = tex->data[tex->get_width() * final_texture_y + final_texture_x];//final_texture_y][final_texture_x];
                    }

                    s->set_depth_data(i, j, depth);
                    s->draw_pixel(j, i, color);
            }

            new_w0 += dw0_x;
            new_w1 += dw1_x;
            new_w2 += dw2_x;
        }

        _w0_start_x = _mm256_add_ps(_w0_start_x, _dw0_y);
        _w1_start_x = _mm256_add_ps(_w1_start_x, _dw1_y);
        _w2_start_x = _mm256_add_ps(_w2_start_x, _dw2_y);
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

typedef struct{
    std::vector<float> world_point_x;
    std::vector<float> world_point_y;
    std::vector<float> world_point_z;

    std::vector<float> converted_point_x;
    std::vector<float> converted_point_y;
    std::vector<float> depth;
}vertexes;

/*
void matrix_mult4x1_simd(float mtx[][4], __m256 _final_points[4]){
	__m256 _points[4] __attribute__((aligned(32)));
	_points[0] = _final_points[0];
	_points[1] = _final_points[1];
	_points[2] = _final_points[2];
	_points[3] = _final_points[3];

    for(int i = 0; i < 4; i++){
        //printf("antes da mula: %d\n", i);
        __m256 _col1 = _mm256_broadcast_ss(&mtx[i][0]);
        __m256 _col2 = _mm256_broadcast_ss(&mtx[i][1]);
        __m256 _col3 = _mm256_broadcast_ss(&mtx[i][2]);
        __m256 _col4 = _mm256_broadcast_ss(&mtx[i][3]);
        _final_points[i] = _mm256_fmadd_ps(_col1, _points[0], _mm256_fmadd_ps(_col2, _points[1], _mm256_fmadd_ps(_col3, _points[2], _mm256_mul_ps(_col4, _points[3]))));
	}
}*/

void matrix_mult4x1_simd(__m256 _mtx[4][4], __m256 _final_points[4]){
	__m256 _points[4] __attribute__((aligned(32)));
	_points[0] = _final_points[0];
	_points[1] = _final_points[1];
	_points[2] = _final_points[2];
	_points[3] = _final_points[3];

    for(int i = 0; i < 4; i++){
        _final_points[i] = _mm256_fmadd_ps(_mtx[i][0], _points[0], _mm256_fmadd_ps(_mtx[i][1], _points[1], _mm256_fmadd_ps(_mtx[i][2], _points[2], _mm256_mul_ps(_mtx[i][3], _points[3]))));
	}
}

void load_values_simd_mtx(float mtx[][4], __m256 simd_mtx[][4]){
    simd_mtx[0][0] = _mm256_broadcast_ss(&mtx[0][0]);
    simd_mtx[0][1] = _mm256_broadcast_ss(&mtx[0][1]);
    simd_mtx[0][2] = _mm256_broadcast_ss(&mtx[0][2]);
    simd_mtx[0][3] = _mm256_broadcast_ss(&mtx[0][3]);

    simd_mtx[1][0] = _mm256_broadcast_ss(&mtx[1][0]);
    simd_mtx[1][1] = _mm256_broadcast_ss(&mtx[1][1]);
    simd_mtx[1][2] = _mm256_broadcast_ss(&mtx[1][2]);
    simd_mtx[1][3] = _mm256_broadcast_ss(&mtx[1][3]);

    simd_mtx[2][0] = _mm256_broadcast_ss(&mtx[2][0]);
    simd_mtx[2][1] = _mm256_broadcast_ss(&mtx[2][1]);
    simd_mtx[2][2] = _mm256_broadcast_ss(&mtx[2][2]);
    simd_mtx[2][3] = _mm256_broadcast_ss(&mtx[2][3]);

    simd_mtx[3][0] = _mm256_broadcast_ss(&mtx[3][0]);
    simd_mtx[3][1] = _mm256_broadcast_ss(&mtx[3][1]);
    simd_mtx[3][2] = _mm256_broadcast_ss(&mtx[3][2]);
    simd_mtx[3][3] = _mm256_broadcast_ss(&mtx[3][3]);
}

void render_mesh(screen* s, mesh *m, camera cam){
    //system("cls");
    //printf("inicio da funcao render mesh\n");
    //printf("tamanho das vertices: %d", m->vertices.size());


    auto lastTime = std::chrono::steady_clock::now();
    float frameTimeSeconds = 1;


    vertexes vertex_data;
    vertex_data.converted_point_x.resize(m->vertices_x.size());
    vertex_data.converted_point_y.resize(m->vertices_x.size());

    vertex_data.world_point_x.resize(m->vertices_x.size());
    vertex_data.world_point_y.resize(m->vertices_x.size());
    vertex_data.world_point_z.resize(m->vertices_x.size());

    vertex_data.depth.resize(m->vertices_x.size());
    //system("cls");
    //printf("tamanho das vertices: %d", m->vertices_x.size());

    //maior gasto de fps:
    float local_to_world_mtx[4][4]; calculate_local_to_world_matrix(m->pos, m->rotation, m->scale, local_to_world_mtx);
    float world_to_view_mtx[4][4]; calculate_world_to_view_matrix(cam.pos, cam.rotation, world_to_view_mtx);
    float local_to_view_mtx[4][4] __attribute__((aligned(32))); matrix_mult4x4(world_to_view_mtx, local_to_world_mtx, local_to_view_mtx);
    float perspective_mtx[4][4] __attribute__((aligned(32))); calculate_perspective_mtx(float(s->screen_width)/float(s->screen_height), cam.fov, cam.cam_near, cam.cam_far, perspective_mtx);

    __m256 _local_to_view_mtx[4][4]; load_values_simd_mtx(local_to_view_mtx, _local_to_view_mtx);
    __m256 _perspective_mtx[4][4]; load_values_simd_mtx(perspective_mtx, _perspective_mtx);

    vector3 point;
    //system("cls");
    int q = 0;

    __m256 _points[4] __attribute__((aligned(32)));
    __m256i _index = _mm256_setr_epi32(0, 3, 6, 9, 12, 15, 18, 21);
    __m256 _onesf = _mm256_set1_ps(1.0f);
    __m256 _twosf = _mm256_set1_ps(2.0f);

    //printf("antes de calcular simd\n");

    for(; q < m->vertices_x.size()-8; q += 8){
        int current_element = q;

        _points[0] = _mm256_loadu_ps(&m->vertices_x[q]);
        _points[1] = _mm256_loadu_ps(&m->vertices_y[q]);
        _points[2] = _mm256_loadu_ps(&m->vertices_z[q]);
        _points[3] = _mm256_set1_ps(1.0f);

        matrix_mult4x1_simd(_local_to_view_mtx, _points);

        _mm256_storeu_ps(&vertex_data.world_point_x[q], _points[0]);
        _mm256_storeu_ps(&vertex_data.world_point_y[q], _points[1]);
        _mm256_storeu_ps(&vertex_data.world_point_z[q], _points[2]);

        matrix_mult4x1_simd(_perspective_mtx, _points);

        __m256 _screen_size = _mm256_set1_ps(float(s->screen_width));
        __m256 _onebyw = _mm256_rcp_ps(_points[3]);

        //(((xs[j] / ws[j])+1)/2)*s->screen_width;
        _points[0] = _mm256_mul_ps(_mm256_div_ps(_mm256_add_ps(_mm256_mul_ps(_points[0], _onebyw), _onesf), _twosf), _screen_size);

        _screen_size = _mm256_set1_ps(float(s->screen_height));

        //((1-(ys[j] / ws[j]))/2)*s->screen_height;
        _points[1] = _mm256_mul_ps(_mm256_div_ps(_mm256_sub_ps(_onesf, _mm256_mul_ps(_points[1], _onebyw)), _twosf), _screen_size);

        _points[2] = _mm256_mul_ps(_points[2], _onebyw);

        _mm256_storeu_ps(&vertex_data.converted_point_x[q], _points[0]);
        _mm256_storeu_ps(&vertex_data.converted_point_y[q], _points[1]);
        _mm256_storeu_ps(&vertex_data.depth[q], _points[3]);
    }

    for(; q < m->vertices_x.size(); q++){
        int current_element = q;
        float pointarr[4] = {m->vertices_x[q], m->vertices_y[q], m->vertices_z[q], 1};

        matrix_mult4x1(local_to_view_mtx, pointarr, pointarr);

        vertex_data.world_point_x[current_element] = pointarr[0];
        vertex_data.world_point_y[current_element] = pointarr[1];
        vertex_data.world_point_z[current_element] = pointarr[2];

        matrix_mult4x1(perspective_mtx, pointarr, pointarr);

        vertex_data.depth[current_element] = pointarr[2] / pointarr[3];

        vertex_data.converted_point_x[current_element] = (((pointarr[0] / pointarr[3])+1)/2)*s->screen_width;
        vertex_data.converted_point_y[current_element] = ((1-(pointarr[1] / pointarr[3]))/2)*s->screen_height;
    }

        auto currentTime = std::chrono::steady_clock::now();

        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        frameTimeSeconds = deltaTime.count();

        std::cout<<"time for matrices stuff:"<<frameTimeSeconds<<"|           "<<std::endl;
        lastTime = currentTime;

        frameTimeSeconds = 1;

     currentTime = std::chrono::steady_clock::now();

    std::string colors = " .:-=+*#%@";
    //draw triangles:
    for(int current_sub_mesh = 0; current_sub_mesh < m->triangles.size(); current_sub_mesh++){
        //printf("current submesh: %d", current_sub_mesh);
        for(int i = 0; i < m->triangles[current_sub_mesh].size(); i+=3){
            //printf("inicio do loop\n");

            int current_vertices[3] = {m->triangles[current_sub_mesh][i+0]-1, m->triangles[current_sub_mesh][i+1]-1, m->triangles[current_sub_mesh][i+2]-1};

            vector3 current_world_triangles[3];
            current_world_triangles[0].set(vertex_data.world_point_x[current_vertices[0]], vertex_data.world_point_y[current_vertices[0]], vertex_data.world_point_z[current_vertices[0]]);
            current_world_triangles[1].set(vertex_data.world_point_x[current_vertices[1]], vertex_data.world_point_y[current_vertices[1]], vertex_data.world_point_z[current_vertices[1]]);
            current_world_triangles[2].set(vertex_data.world_point_x[current_vertices[2]], vertex_data.world_point_y[current_vertices[2]], vertex_data.world_point_z[current_vertices[2]]);

            vector2 current_converted_points[3];
            current_converted_points[0].set(vertex_data.converted_point_x[current_vertices[0]], vertex_data.converted_point_y[current_vertices[0]]);
            current_converted_points[1].set(vertex_data.converted_point_x[current_vertices[1]], vertex_data.converted_point_y[current_vertices[1]]);
            current_converted_points[2].set(vertex_data.converted_point_x[current_vertices[2]], vertex_data.converted_point_y[current_vertices[2]]);

            //check if the triangle is counter clockwise
            if(!is_triangle_ccw(current_converted_points[0],
                                current_converted_points[1],
                                current_converted_points[2]))
                continue;

            vector3 zvalues;
            zvalues.set(current_world_triangles[0].z, current_world_triangles[1].z, current_world_triangles[2].z);
            //check if the z values are behing near plane
            if(zvalues.x < cam.cam_near || zvalues.y < cam.cam_near || zvalues.z < cam.cam_near)
                continue;

            vector3 light_direction;
            light_direction.set(1, 1, 0);
            light_direction = light_direction.normalized();

            //inverse rotating the light by the camera angle to fix the light angle
            light_direction = rotated_by_y(light_direction, -cam.rotation.y);
            light_direction = rotated_by_x(light_direction, -cam.rotation.x);

            vector3 current_normal = triangle_normal(current_world_triangles[0], current_world_triangles[1], current_world_triangles[2]);
            float dot = current_normal.dot(light_direction);

            if(dot < -1 || dot > 1) continue;
            if(dot < 0) dot = 0;
            if(dot > 1) dot = 1;

            float light_level = std::min(dot+0.23, 1.0);

            char color = colors[int(range(0, colors.length()-1, 0, 1, light_level))];

            vector2 tex_pos1;
            vector2 tex_pos2;
            vector2 tex_pos3;
            if(m->have_texture()){
                tex_pos1 = m->vertex_texture[m->vertex_texture_indices[current_sub_mesh][i+0]-1];
                tex_pos2 = m->vertex_texture[m->vertex_texture_indices[current_sub_mesh][i+1]-1];
                tex_pos3 = m->vertex_texture[m->vertex_texture_indices[current_sub_mesh][i+2]-1];
            }

            //printf("antes de draw_triangle\n");
            draw_triangle(s, current_converted_points[0],
                             current_converted_points[1],
                             current_converted_points[2],
                             tex_pos1,
                             tex_pos2,
                             tex_pos3,
                             color, zvalues, &m->materials[m->get_current_material(current_sub_mesh)].albedo_texture, m->have_texture());
        }
    }
    deltaTime = currentTime - lastTime;
    frameTimeSeconds = deltaTime.count();
    std::cout<<"time for triangle:"<<frameTimeSeconds<<"|             "<<std::endl;
    lastTime = currentTime;

}
