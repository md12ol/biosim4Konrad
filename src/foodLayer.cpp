// foodLayer.cpp

#include <functional>
#include <cassert>
#include <iostream>
#include "foodLayer.h"
#include "simulator.h"
#include "grid.h"

namespace BS {

    // Allocates space for the 2D grid
    void FoodLayer::init(uint16_t sizeX, uint16_t sizeY)
    {
        auto col = Column(sizeY);
        data = std::vector<Column>(sizeX, col);
    }

    // Fills the food layer depending on the locations
    void FoodLayer::fill() {
        for (int16_t x = 0; x < p.sizeX; ++x) {
            for (int16_t y = 0; y < p.sizeY; ++y) {
                if (grid.at(x,y) == FOODAREA || grid.at(x,y) == SAFEFOODAREA) {
                    if (p.foodRegrowth == 0) {
                        foodLayer.set(x, y, p.foodPerTile);
                    } else {
                        foodLayer.set(x, y, std::min(static_cast<uint16_t>(p.foodPerTile), static_cast<uint16_t>(round(p.foodRegrowth * foodLayer.at(x, y)))));
                    }
                }
            }
        }
    }

    // Decrements the food layer at a specific location by 1
    void FoodLayer::decrement(uint16_t x, uint16_t y) {
        foodLayer.set(x, y, foodLayer.at(x, y) - 1);

        if (foodLayer.at(x, y) == FOOD_MIN) {

            auto &foodAreaLocations = grid.getFoodAreaLocations();
            auto &safeFoodAreaLocations = grid.getSafeFoodAreaLocations();
            auto &safeAreaLocations = grid.getSafeAreaLocations();

            // in case the food cell is a food area: remove coordinates from foodAreaLocations
            for (Coord loc : foodAreaLocations) {
                if (loc == Coord(x, y)) {
                    std::remove(foodAreaLocations.begin(), foodAreaLocations.end(), loc);
                    foodAreaLocations.pop_back();
                    break;
                }
            }

            // in case the food cell is a safe food area: remove coordinates from safeFoodAreaLocations
            // and add them to the safeAreaLocations
            for (Coord loc : safeFoodAreaLocations) {
                //std::cout << "Goes into safe food area loop" << std::endl;
                if (loc == Coord(x, y)) {
                    //std::cout << "Goes into safe food area case" << std::endl;
                    std::remove(safeFoodAreaLocations.begin(), safeFoodAreaLocations.end(), loc);
                    safeFoodAreaLocations.pop_back();
                    safeAreaLocations.push_back(loc);
                    break;
                }
            }
        }
    }

}
