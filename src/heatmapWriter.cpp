#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
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
    cimg_library::CImgList<uint8_t> heatmapImageList;

    // Initializes a heatmap with size depending on Sensors, Neurons and Actions.
    void initHeatmap() {

        // Initialize the heatmap depending on the number of neurons
        std::cout << "heatmap initialized" << std::endl;
        heatmap.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounter.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
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
            }
        }

        // Calculate the weights in the fields
        for (uint16_t index = 1; index <= p.population; ++index) {
            const Indiv &indiv = peeps[index];

            // Check the neuronal net of an indiv
            for (auto &conn : indiv.nnet.connections) {
                // Check the source (row) of the connection
                if (conn.sourceType == SENSOR) {
                    heatmapRow = (Sensor)conn.sourceNum;
                    assert(heatmapRow < NUM_SENSES);
                } else if (conn.sourceType == NEURON) {
                    assert(conn.sourceNum < p.maxNumberNeurons);
                    heatmapRow = NUM_SENSES + conn.sourceNum;
                }

                // Check the sink (column) of the connection
                if (conn.sinkType == ACTION) {
                    assert((Action)conn.sinkNum < NUM_ACTIONS);
                    // Display the Actions after the neurons in the columns
                    heatmapCol = (Action)conn.sinkNum + p.maxNumberNeurons;
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
            }
        }

        // Calculate the average of each heatmap cell
        for (int x = 0; x < heatmap.sizeX(); x++) {
            for (int y = 0; y < heatmap.sizeY(); y++) {
                if (heatmapCounter.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int average = heatmap.at(x, y) / heatmapCounter.at(x, y);
                    heatmap.set(x, y, average);
                    assert(heatmap.at(x, y) == average);
                }
            }
        }
    }

    // Pushes a heatmap image into the list, which is used to create the avi-video (the origin of coordinates
    // in CImg.h is in the upper left corner).
    void saveHeatmapImage(unsigned generation) {
        using namespace cimg_library;
        int heatmapImageScale = 32;

        CImg<uint8_t> image((heatmap.sizeY() + 1) * heatmapImageScale,
            (heatmap.sizeX() + 1) * heatmapImageScale,
            1,
            3,
            255);

        uint8_t colorPositive[3] = {0, 255, 0};
        uint8_t colorNegative[3] = {255, 0, 0};
        uint8_t colorTextForeground[3] = {0, 0, 0};
        uint8_t colorTextBackground[3] = {255, 255, 255};

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
            colorTextForeground,
            1.0
            );

        // Display the strings of sensor and internal neurons
        for (int x = 0; x < heatmap.sizeX(); ++x) {
            if (x < NUM_SENSES) {
                image.draw_text(
                    0,
                    (x + 1) * heatmapImageScale,
                    sensorShortName((Sensor)x).c_str(),
                    colorTextForeground,
                    colorTextBackground,
                    1.0,
                    heatmapImageScale / 3
                    );
            } else {
                // Write text for internal neurons
                image.draw_text(
                    0,
                    (x + 1) * heatmapImageScale,
                    "N",
                    colorTextForeground,
                    colorTextBackground,
                    1.0,
                    heatmapImageScale / 3);
            }
        }

        // Display the strings for actions and internal neurons
        for (int y = 0; y < heatmap.sizeY(); ++y) {
            if (y < p.maxNumberNeurons) {
                image.draw_text(
                    (y + 1) * heatmapImageScale,
                    0,
                    "N",
                    colorTextForeground,
                    colorTextBackground,
                    1.0,
                    heatmapImageScale / 3
                    );
            } else {
                image.draw_text(
                    (y + 1) * heatmapImageScale,
                    0,
                    actionShortName((Action)(y - p.maxNumberNeurons)).c_str(),
                    colorTextForeground,
                    colorTextBackground,
                    1.0,
                    heatmapImageScale / 3
                    );
            }
        }

        for (int x = 0; x < heatmap.sizeX(); x++) {
            for (int y = 0; y < heatmap.sizeY(); y++) {
                if (heatmap.at(x, y) > 0) {
                    image.draw_circle(
                        (y+1) * heatmapImageScale,
                        (x+1) * heatmapImageScale,
                        heatmapImageScale / 2,
                        colorPositive,
                        (double)heatmap.at(x, y) / 32767);
                } else if (heatmap.at(x, y) < 0) {
                    image.draw_circle(
                        (y+1) * heatmapImageScale,
                        (x+1) * heatmapImageScale,
                        heatmapImageScale / 2,
                        colorNegative,
                        (double)heatmap.at(x, y) / -32767);
                } else {
                    // Do nothing since heatmap at this position is 0
                    // -> weights cancel themselves out or individuals do not use this connection
                }
            }
        }
        heatmapImageList.push_back(image);

        // Clear the heatmap and heatmap counter after saving an image
        heatmap.zeroFill();
        heatmapCounter.zeroFill();
    }

    // Takes the list of all heatmap images and creates a video with one frame for each generation.
    void createHeatmapVideo(unsigned generation) {
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
    }
}
