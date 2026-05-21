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
    cimg_library::CImgList<uint8_t> heatmapImageList;

    // Initializes a heatmap with size depending on Sensors, Neurons and Actions.
    void initHeatmap() {

        // Initialize the heatmap depending on the number of neurons
        std::cout << "heatmap initialized" << std::endl;
        heatmap.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
    }

    void fillHeatmap() {
        // Access the heatmap at the correct indices
        uint16_t heatmapRow = -1;
        uint16_t heatmapCol = -1;

        // Check that heatmap is empty
        for (int x = 0; x < heatmap.sizeX(); x++) {
            for (int y = 0; y < heatmap.sizeY(); y++) {
                assert(heatmap.at(x, y) == 0);
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

                // Get the previous value for the connection in the heatmap
                const int previousValue = heatmap.at(heatmapRow, heatmapCol);
                // Set heatmap to the new value
                heatmap.set(heatmapRow, heatmapCol, previousValue + conn.weight);
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

        uint8_t colorPositive[3];
        uint8_t colorNegative[3];
        uint8_t colorTextForeground[3];
        uint8_t colorTextBackground[3];
        std::stringstream imageFilename;
        imageFilename << p.imageDir << "frame-"
        << std::setfill('0') << std::setw(6) << generation
        << ".png";

        colorPositive[0] = 0;
        colorPositive[1] = 255;
        colorPositive[2] = 0;
        colorNegative[0] = 255;
        colorNegative[1] = 0;
        colorNegative[2] = 0;
        colorTextForeground[0] = 0;
        colorTextForeground[1] = 0;
        colorTextForeground[2] = 0;
        colorTextBackground[0] = 255;
        colorTextBackground[1] = 255;
        colorTextBackground[2] = 255;
        // ToDo: Find a good scale for visibility in the video.
        // ToDo: Average the weights for the neuronal connections (maybe a second heatmap to count number of
        // specific connections).
        // ToDo: Prevent opacity from getting rounded down or up.
        // ToDo: Choose a good video format (maybe mp4 is better because it is less susceptible to errors).
        // ToDo: Make sure that creating a frame is thread-safe (maybe use an immutable copy of the population
        // when function is called).



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
                image.draw_circle(
                    (y+1) * heatmapImageScale,
                    (x+1) * heatmapImageScale,
                    heatmapImageScale / 2,
                    heatmap.at(x, y) > 0 ? colorPositive : colorNegative,
                    std::abs(std::tanh(heatmap.at(x, y))));
            }
        }
        heatmapImageList.push_back(image);

        // Clear the heatmap after saving an image
        heatmap.zeroFill();
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
