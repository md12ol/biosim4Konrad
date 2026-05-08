#include <condition_variable>
#include <chrono>
#include "simulator.h"
#include "heatmapVector.h"
#define cimg_use_opencv 1
#define cimg_display 0
#include "CImg.h"

namespace BS {

    // Y = Length of a row
    // X = Number of rows [x][0]
    void HeatmapVector::init(uint16_t sizeX, uint16_t sizeY) {
        auto row = Row(sizeY);
        data = std::vector<Row>(sizeX, row);
    }
}