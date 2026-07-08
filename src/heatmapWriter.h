#ifndef HEATMAPWRITER_H_INCLUDED
#define HEATMAPWRITER_H_INCLUDED

#include <cstdint>
#include <vector>
#include <functional>
#include <iterator>

namespace BS {

    // This holds all data needed to create one heatmap frame. The data is
    // cached in this structure so that the heatmap writer can work on it in
    // a separate thread while the main thread starts a new simstep.

    /*
     * This class is a 2-dimensional vector, which will be filled with the weights of neuronal connections
     * [Sensor0, Action0], [Sensor0, Action1], [Sensor0, Action2] ... [Sensor0, ActionN]
     * [Sensor1, Action0], [Sensor1, Action1], [Sensor1, Action2] ... [Sensor1, ActionN]
     *                                         .
     *                                         .
     *                                         .
     * [SensorN, Action0], [Sensor1, Action1], [Sensor1, Action2] ... [Sensor2, ActionN]
     */

    struct HeatmapFrameData {
        unsigned generation;

    };

    void initHeatmap();
    void fillHeatmap();
    void saveHeatmapImages(unsigned generation);
    void createHeatmapVideos(unsigned generation);

    void initHeatmapMultipleRuns();
    void fillHeatmapMultipleRuns();
    void saveHeatmapImagesMultipleRuns(unsigned run);
}
#endif //HEATMAPWRITER_H_INCLUDED
