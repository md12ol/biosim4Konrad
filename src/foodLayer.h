#ifndef FOODLAYER_H_INCLUDED
#define FOODLAYER_H_INCLUDED

#include <vector>
#include <utility>
#include <cstdint>
#include "basicTypes.h"

// Container for food.

namespace BS {
    constexpr unsigned FOOD_MIN = 0;
    constexpr unsigned FOOD_MAX = UINT16_MAX;

    class FoodLayer {
    public:
        struct Column {
            Column(uint16_t numRows) : data { std::vector<uint16_t>(numRows, 0) } { }
            uint16_t& operator[](uint16_t rowNum) { return data[rowNum]; }
            uint16_t operator[](uint16_t rowNum) const { return data[rowNum]; }
            void zeroFill() { std::fill(data.begin(), data.end(), 0); }
            size_t size() const { return data.size(); }
        private:
            std::vector<uint16_t> data;
        };


        void init(uint16_t sizeX, uint16_t sizeY);
        void zeroFill() { for (Column &column : data) column.zeroFill(); }
        void fill();
        void decrement(uint16_t x, uint16_t y);
        uint16_t sizeX() const { return data.size(); }
        uint16_t sizeY() const { return data[0].size(); }
        uint16_t at(Coord loc) const { return data[loc.x][loc.y]; }
        uint16_t at(uint16_t x, uint16_t y) const { return data[x][y]; }
        void set(Coord loc, uint16_t val) { data[loc.x][loc.y] = val; }
        void set(uint16_t x, uint16_t y, uint16_t val) { data[x][y] = val; }
        // Direct access:
        Column & operator[](uint16_t columnXNum) { return data[columnXNum]; }
        const Column & operator[](uint16_t columnXNum) const { return data[columnXNum]; }

    private:
        std::vector<Column> data;
    };
}




#endif // FOODLAYER_H_INCLUDED
