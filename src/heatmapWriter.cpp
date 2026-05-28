#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <condition_variable>
#include "simulator.h"
#include "heatmapWriter.h"
#include "heatmapVector.h"
#define cimg_use_opencv 1
#define cimg_display 0
#include "CImg.h"
#include <cassert>

namespace BS {

    HeatmapVector heatmap;
    HeatmapVector heatmapCounter;
    HeatmapVector heatmapSum;
    cimg_library::CImgList<uint8_t> heatmapImageList;
    cimg_library::CImgList<uint8_t> heatmapCounterImageList;
    cimg_library::CImgList<uint8_t> heatmapSumImageList;

    constexpr uint8_t colorGreen[3] = {0, 255, 0};
    constexpr uint8_t colorRed[3] = {255, 0, 0};
    constexpr uint8_t colorBlack[3] = {0, 0, 0};
    constexpr uint8_t colorWhite[3] = {255, 255, 255};

    // Initializes a heatmap with size depending on Sensors, Neurons and Actions.
    void initHeatmap() {

        // Initialize the heatmap depending on the number of neurons
        std::cout << "heatmap initialized" << std::endl;
        heatmap.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounter.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSum.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
    }

    void fillHeatmap() {
        // Access the heatmap at the correct indices
        uint16_t heatmapRow = 10000;
        uint16_t heatmapCol = 10000;

        // Check that heatmap and heatmapCounter are empty
        for (int x = 0; x < heatmap.sizeX(); x++) {
            for (int y = 0; y < heatmap.sizeY(); y++) {
                assert(heatmap.at(x, y) == 0);
                assert(heatmapCounter.at(x, y) == 0);
                assert(heatmapSum.at(x, y) == 0);
            }
        }

        // Calculate the weights in the fields
        for (uint16_t index = 1; index <= p.population; ++index) {
            const Indiv &indiv = peeps[index];

            // Check the neuronal net of an indiv
            for (auto &conn : indiv.nnet.connections) {
                // Check the source (row) of the connection
                if (conn.sourceType == SENSOR) {
                    heatmapRow = static_cast<Sensor>(conn.sourceNum);
                    assert(heatmapRow < NUM_SENSES);
                } else if (conn.sourceType == NEURON) {
                    assert(conn.sourceNum < p.maxNumberNeurons);
                    heatmapRow = NUM_SENSES + conn.sourceNum;
                }

                // Check the sink (column) of the connection
                if (conn.sinkType == ACTION) {
                    assert(static_cast<Action>(conn.sinkNum) < NUM_ACTIONS);
                    // Display the Actions after the neurons in the columns
                    heatmapCol = static_cast<Action>(conn.sinkNum) + p.maxNumberNeurons;
                } else if (conn.sinkType == NEURON) {
                    assert(conn.sinkNum < p.maxNumberNeurons);
                    heatmapCol = conn.sinkNum;
                }

                assert(heatmapCol < 10000);
                assert(heatmapRow < 10000);

                // Get the previous value for the connection in the heatmap
                const int previousValue = heatmap.at(heatmapRow, heatmapCol);
                const int previousCount = heatmapCounter.at(heatmapRow, heatmapCol);
                // Set heatmap to the new value
                heatmap.set(heatmapRow, heatmapCol, previousValue + conn.weight);
                heatmapCounter.set(heatmapRow, heatmapCol, previousCount + 1);
                heatmapSum.set(heatmapRow, heatmapCol, previousValue + conn.weight);
            }
        }

        // Calculate the average of each heatmap cell
        // and also divide each cell of heatmapSum with the absolute maximum sum of weights
        for (int x = 0; x < heatmap.sizeX(); x++) {
            for (int y = 0; y < heatmap.sizeY(); y++) {
                if (heatmapCounter.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int average = heatmap.at(x, y) / heatmapCounter.at(x, y);
                    heatmap.set(x, y, average);
                }
            }
        }
    }

    // Draws the images for a specific heatmap
    void drawImage(const unsigned generation, const HeatmapVector& hm,
        cimg_library::CImgList<uint8_t>& heatmapList, const int heatmapImageScale, const std::string &type) {
        using namespace cimg_library;

        CImg<uint8_t> image((hm.sizeX() + 1) * heatmapImageScale,
            (hm.sizeY() + 1) * heatmapImageScale,
            1,
            3,
            255);

        std::stringstream imageFilename;
        imageFilename << p.imageDir << "frame-"
        << std::setfill('0') << std::setw(6) << generation
        << ".png";

        // Display a grid for better visibility
        image.draw_grid(
            heatmapImageScale,
            heatmapImageScale,
            heatmapImageScale / 2,
            heatmapImageScale / 2,
            false,
            false,
            colorBlack,
            1.0);

        // Display the strings of sensor and internal neurons
        for (int x = 0; x < hm.sizeX(); ++x) {
            if (x < NUM_SENSES) {
                image.draw_text(
                    (x + 1) * heatmapImageScale,
                    0,
                    sensorName(static_cast<Sensor>(x)).c_str(),
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            } else {
                // Write text for internal neurons
                image.draw_text(
                    (x + 1) * heatmapImageScale,
                    0,
                    "N",
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            }
        }

        // Display the strings for actions and internal neurons
        for (int y = 0; y < hm.sizeY(); ++y) {
            if (y < p.maxNumberNeurons) {
                image.draw_text(
                    0,
                    (y + 1) * heatmapImageScale,
                    "N",
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            } else {
                image.draw_text(
                    0,
                    (y + 1) * heatmapImageScale,
                    actionName(static_cast<Action>(y - p.maxNumberNeurons)).c_str(),
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            }
        }

        // Draw circles for values in the heatmap
        if (type == "average") {
            for (int x = 0; x < hm.sizeX(); x++) {
                for (int y = 0; y < hm.sizeY(); y++) {
                    if (hm.at(x, y) > 0) {
                        image.draw_circle(
                            (x+1) * heatmapImageScale,
                            (y+1) * heatmapImageScale,
                            heatmapImageScale / 2,
                            colorGreen,
                            static_cast<double>(hm.at(x, y)) / 32767);
                    } else if (hm.at(x, y) < 0) {
                        image.draw_circle(
                            (x+1) * heatmapImageScale,
                            (y+1) * heatmapImageScale,
                            heatmapImageScale / 2,
                            colorRed,
                            static_cast<double>(hm.at(x, y)) / -32767);
                    } else {
                        // Do nothing since heatmap at this position is 0
                        // -> weights cancel themselves out or individuals do not use this connection
                    }
                }
            }
        } else if (type == "counter") {
            for (int x = 0; x < heatmap.sizeX(); x++) {
                for (int y = 0; y < heatmap.sizeY(); y++) {
                    if (hm.at(x, y) > 0) {
                        image.draw_circle(
                            (x+1) * heatmapImageScale,
                            (y+1) * heatmapImageScale,
                            heatmapImageScale / 2,
                            colorGreen,
                            static_cast<double>(hm.at(x, y)) / p.population);
                    }
                }
            }
        } else if (type == "sum") {
            const int absoluteMaximum = hm.getAbsoluteMaximum();
            for (int x = 0; x < hm.sizeX(); x++) {
                for (int y = 0; y < hm.sizeY(); y++) {
                    image.draw_circle(
                    (x+1) * heatmapImageScale,
                    (y+1) * heatmapImageScale,
                    heatmapImageScale / 2,
                    hm.at(x, y) > 0 ? colorGreen : colorRed,
                    hm.at(x, y) > 0 ? static_cast<double>(hm.at(x, y)) / absoluteMaximum : static_cast<double>(hm.at(x, y)) / -absoluteMaximum);
                }
            }
        }

        heatmapList.push_back(image);
    }


    // Pushes the heatmap images into their lists (the origin of coordinates
    // in CImg.h is in the upper left corner).
    void saveHeatmapImages(const unsigned generation) {

        drawImage(generation, heatmap, heatmapImageList, 32, "average");
        drawImage(generation, heatmapCounter, heatmapCounterImageList, 32, "counter");
        drawImage(generation, heatmapSum, heatmapSumImageList, 32, "sum");

        // Clear the heatmap and heatmap counter after saving an image
        heatmap.zeroFill();
        heatmapCounter.zeroFill();
        heatmapSum.zeroFill();
    }

    // Takes the list of all heatmap images and creates a video with one frame for each generation.
    void createHeatmapVideos(unsigned generation) {

        if (heatmapImageList.size() > 0) {
            std::cout << "Frames are in the heatmapImageList" << std::endl;
            std::cout << "Number of frames in the heatmapImageList: " << heatmapImageList.size() << std::endl;
            std::stringstream videoFilename;
            videoFilename << p.imageDir.c_str() << "heatmap-"
            << std::setfill('0') << std::setw(6) << generation << ".mp4";
            heatmapImageList.save_video(videoFilename.str().c_str(),
                1,
                "mp4v",
                false);
        }
        heatmapImageList.clear();

        if (heatmapCounterImageList.size() > 0) {
            std::cout << "Frames are in the heatmapCounterImageList" << std::endl;
            std::cout << "Number of frames in the heatmapCounterImageList: " << heatmapCounterImageList.size() << std::endl;
            std::stringstream videoFilename;
            videoFilename << p.imageDir.c_str() << "heatmapCounter-"
            << std::setfill('0') << std::setw(6) << generation << ".mp4";
            heatmapCounterImageList.save_video(videoFilename.str().c_str(),
                1,
                "mp4v",
                false);
        }
        heatmapCounterImageList.clear();

        if (heatmapSumImageList.size() > 0) {
            std::cout << "Frames are in the heatmapSumImageList" << std::endl;
            std::cout << "Number of frames in the heatmapSumImageList: " << heatmapSumImageList.size() << std::endl;
            std::stringstream videoFilename;
            videoFilename << p.imageDir.c_str() << "heatmapSum-"
            << std::setfill('0') << std::setw(6) << generation << ".mp4";
            heatmapSumImageList.save_video(videoFilename.str().c_str(),
                1,
                "mp4v",
                false);
        }
        heatmapSumImageList.clear();
    }
}
