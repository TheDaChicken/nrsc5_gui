//
// Created by TheDaChicken on 11/11/2025.
//

#ifndef NRSC5_GUI_FRAME_H
#define NRSC5_GUI_FRAME_H
#include <vector>

struct RadioFrame
{
	std::vector<char> data;
	std::size_t frame_size = 0;
	int dropped_samples;
};

#endif //NRSC5_GUI_FRAME_H