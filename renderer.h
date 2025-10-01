#pragma once
#include <vector>
#include "vector.h"
#include "screen.h"
#include "mesh.h"
#include "camera.h"

vector2 convert_3d_to_2d(vector3 a, float fov);
void draw_points(screen* s, std::vector<float> points, vector3 pos);
void draw_triangle(screen* s, vector2 a, vector2 b, vector2 c);
void render_mesh(screen* s, mesh m, camera cam);
