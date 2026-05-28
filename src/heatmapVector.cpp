#include <condition_variable>
#include "heatmapVector.h"

namespace BS {

    // Y = Length of a row
    // X = Number of rows [x][0]
    void HeatmapVector::init(uint16_t sizeX, uint16_t sizeY) {
        auto row = Row(sizeY);
        data = std::vector<Row>(sizeX, row);
    }

    int HeatmapVector::getAbsoluteMaximum() const {
        int absoluteMaximum = 0;
        for (int x = 0; x < this->sizeX(); x++) {
            for (int y = 0; y < this->sizeY(); y++) {
                if (std::abs(data[x][y]) > absoluteMaximum) {
                    absoluteMaximum = std::abs(data[x][y]);
                }
            }
        }
        return absoluteMaximum;
    }
}
