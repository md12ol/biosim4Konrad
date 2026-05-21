#include <condition_variable>
#include "heatmapVector.h"

namespace BS {

    // Y = Length of a row
    // X = Number of rows [x][0]
    void HeatmapVector::init(uint16_t sizeX, uint16_t sizeY) {
        auto row = Row(sizeY);
        data = std::vector<Row>(sizeX, row);
    }
}