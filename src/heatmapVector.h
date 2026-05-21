#ifndef HEATMAPVECTOR_H_INCLUDED
#define HEATMAPVECTOR_H_INCLUDED

#include <cstdint>
#include <vector>
#include <functional>
#include <iterator>

namespace BS {
    class HeatmapVector {
    public:
        struct Row {
            Row(uint16_t numRows) : data { std::vector<int16_t>(numRows, 0) } { }
            void zeroFill() { std::fill(data.begin(), data.end(), 0); }
            int16_t& operator[](uint16_t columnNum) { return data.at(columnNum); }
            int16_t operator[](uint16_t columnNum) const { return data.at(columnNum); }
            size_t size() const { return data.size(); }
        private:
            std::vector<int16_t> data;
        };

        void init(uint16_t sizeX, uint16_t sizeY);
        void zeroFill() { for (Row &row : data) row.zeroFill(); }
        uint16_t sizeX() const { return data.size(); }
        uint16_t sizeY() const { return data[0].size(); }
        int16_t at(uint16_t x, uint16_t y) const { return data[x][y]; }
        void set(uint16_t x, uint16_t y, int16_t value) { data[x][y] = value; }
        // Direct Access
        Row & operator[](uint16_t rowXNum) { return data.at(rowXNum); }
        const Row & operator[](uint16_t rowXNum) const { return data.at(rowXNum); }

    private:
        std::vector<Row> data;
    };
}
#endif //HEATMAPVECTOR_H_INCLUDED
