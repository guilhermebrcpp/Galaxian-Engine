#pragma once
#include <vector>
#include "vector.h"

void prepare_screen_for_rendering(int screen_width, int screen_height);
void calculate_recommended_resolutions(int start_y, int step, std::vector<vector2i> *resolutions);
vector2i ask_and_get_user_resolution();
std::string ask_and_get_user_model();
