// createSafeArea.cpp

#include <cassert>
#include "simulator.h"

namespace BS {

    // This generates safeArea points, which are grid locations with value
    // SAFEAREA. A list of safeArea locations is safed in private member
    // Grid::safeAreaLocation and, for some scenarios, Grid::safeAreaCenters.
    // Those members are available read-only with Grid::getSafeAreaLocations().
    // This function assumes an empty grid. This is typically called by
    // the main simulator thread after Grid::init() or Grid::zeroFill().

    // This file typically is under constant development and change for
    // specific scenarios.

    void Grid::createSafeArea(unsigned safeAreaType) {
        safeAreaLocations.clear();
        safeAreaCenters.clear();

        auto drawBox = [&](int16_t minX, int16_t minY, int16_t maxX, int16_t maxY) {
            for (int16_t x = minX; x <= maxX; ++x) {
                for (int16_t y = minY; y <= maxY; ++y) {
                    grid.set(x, y, SAFEAREA);
                    safeAreaLocations.push_back( {x, y} );
                }
            }
        };

        switch (safeAreaType) {
        case 0:
           return;

        // Spots, specified number, radius, locations
        case 1:
            {
                unsigned numberOfLocations = 5;
                float radius = 5.0;

                auto f = [&](Coord loc) {
                    grid.set(loc, SAFEAREA);
                    safeAreaLocations.push_back(loc);
                };

                unsigned verticalSliceSize = p.sizeY / (numberOfLocations + 1);

                for (unsigned n = 1; n <= numberOfLocations; ++n) {
                    Coord loc = { (int16_t)(p.sizeX / 2),
                                  (int16_t)(n * verticalSliceSize) };
                    visitNeighborhood(loc, radius, f);
                    safeAreaCenters.push_back(loc);
                }
            }
            break;

        default:
            assert(false);
        }
    }
}
