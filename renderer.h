#pragma once
#include <vector>
#include "vector.h"
#include "screen.h"
#include "mesh.h"
#include "camera.h"
#include "texture.h"

vector2 convert_3d_to_2d(vector3 a, float fov);
void render_mesh(screen* s, mesh *m, camera cam);
