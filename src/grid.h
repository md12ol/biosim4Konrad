#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

// The grid is the 2D arena where the agents live.

#include <cstdint>
#include <vector>
#include <functional>
#include "basicTypes.h"

namespace BS {

// Grid is a somewhat dumb 2D container of unsigned 16-bit values.
// Grid understands that the elements are either EMPTY, BARRIER, or
// otherwise an index value into the peeps container.
// The elements are allocated and cleared to EMPTY in the ctor.
// Prefer .at() and .set() for random element access. Or use Grid[x][y]
// for direct access where the y index is the inner loop.
// Element values are not otherwise interpreted by class Grid.

const uint16_t EMPTY = 0; // Index value 0 is reserved
const uint16_t BARRIER = 0xffff;
const uint16_t SAFEAREA = 0xfffe;
const uint16_t FOODAREA = 0xfffd;

class Grid {
public:
    // Column order here allows us to access grid elements as data[x][y]
    // while thinking of x as column and y as row
    struct Column {
        Column(uint16_t numRows) : data { std::vector<uint16_t>(numRows, 0) } { }
        void zeroFill() { std::fill(data.begin(), data.end(), 0); }
        uint16_t& operator[](uint16_t rowNum) { return data[rowNum]; }
        uint16_t operator[](uint16_t rowNum) const { return data[rowNum]; }
        size_t size() const { return data.size(); }
    private:
        std::vector<uint16_t> data;
    };

    void init(uint16_t sizeX, uint16_t sizeY);
    void zeroFill() { for (Column &column : data) column.zeroFill(); }
    uint16_t sizeX() const { return data.size(); }
    uint16_t sizeY() const { return data[0].size(); }
    bool isInBounds(Coord loc) const { return loc.x >= 0 && loc.x < sizeX() && loc.y >= 0 && loc.y < sizeY(); }
    bool isEmptyAt(Coord loc) const { return at(loc) == EMPTY; }
    bool isBarrierAt(Coord loc) const { return at(loc) == BARRIER; }
    bool isSafeAreaAt(Coord loc) const { return at(loc) == SAFEAREA; }
    bool isFoodAreaAt(Coord loc) const { return at(loc) == FOODAREA; }
    // Occupied means an agent is living there.
    bool isOccupiedAt(Coord loc) const { return at(loc) != EMPTY && at(loc) != BARRIER && at(loc) != SAFEAREA && at(loc) != FOODAREA; }
    bool isBorder(Coord loc) const { return loc.x == 0 || loc.x == sizeX() - 1 || loc.y == 0 || loc.y == sizeY() - 1; }
    uint16_t at(Coord loc) const { return data[loc.x][loc.y]; }
    uint16_t at(uint16_t x, uint16_t y) const { return data[x][y]; }

    void set(Coord loc, uint16_t val) { data[loc.x][loc.y] = val; }
    void set(uint16_t x, uint16_t y, uint16_t val) { data[x][y] = val; }
    Coord findValidLocation(bool isMouse) const;
    void createBarrier(unsigned barrierType);
    void createSafeArea(unsigned safeAreaType);
    void createFoodArea(unsigned foodAreaType);
    const std::vector<Coord> &getBarrierLocations() const { return barrierLocations; }
    const std::vector<Coord> &getBarrierCenters() const { return barrierCenters; }
    const std::vector<Coord> &getSafeAreaLocations() const { return safeAreaLocations; }
    const std::vector<Coord> &getSafeAreaCenters() const { return safeAreaCenters; }
    const std::vector<Coord> &getFoodAreaLocations() const { return foodAreaLocations; }
    const std::vector<Coord> &getFoodAreaCenters() const { return foodAreaCenters; }
    // Direct access:
    Column & operator[](uint16_t columnXNum) { return data[columnXNum]; }
    const Column & operator[](uint16_t columnXNum) const { return data[columnXNum]; }
private:
    std::vector<Column> data;
    std::vector<Coord> barrierLocations;
    std::vector<Coord> barrierCenters;
    std::vector<Coord> safeAreaLocations;
    std::vector<Coord> safeAreaCenters;
    std::vector<Coord> foodAreaLocations;
    std::vector<Coord> foodAreaCenters;
};

extern void visitNeighborhood(Coord loc, float radius, std::function<void(Coord)> f);
extern bool isWithinSpecifiedArea(Coord loc, uint16_t specifiedType);
extern void unitTestGridVisitNeighborhood();

} // end namespace BS

#endif // GRID_H_INCLUDED
