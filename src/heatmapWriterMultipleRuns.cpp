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
#define cimg_use_png 1
#include "CImg.h"
#include <cassert>

namespace BS {

    HeatmapVector heatmapMultipleRuns;
    HeatmapVector heatmapMultipleRunsMice;
    HeatmapVector heatmapMultipleRunsCats;
    HeatmapVector heatmapCounterMultipleRuns;
    HeatmapVector heatmapCounterMultipleRunsMice;
    HeatmapVector heatmapCounterMultipleRunsCats;
    HeatmapVector heatmapSumMultipleRuns;
    HeatmapVector heatmapSumMultipleRunsMice;
    HeatmapVector heatmapSumMultipleRunsCats;

    constexpr uint8_t colorGreen[3] = {0, 255, 0};
    constexpr uint8_t colorRed[3] = {255, 0, 0};
    constexpr uint8_t colorBlack[3] = {0, 0, 0};
    constexpr uint8_t colorWhite[3] = {255, 255, 255};

    // Initializes a heatmap with size depending on Sensors, Neurons and Actions.
    void initHeatmapMultipleRuns() {

        // Initialize the heatmap depending on the number of neurons
        std::cout << "heatmap initialized" << std::endl;
        heatmapMultipleRuns.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapMultipleRunsMice.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapMultipleRunsCats.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounterMultipleRuns.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounterMultipleRunsMice.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounterMultipleRunsCats.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSumMultipleRuns.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSumMultipleRunsMice.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSumMultipleRunsCats.init(NUM_SENSES + p. maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
    }

    void fillHeatmapMultipleRuns() {
        // Access the heatmap at the correct indices
        uint16_t heatmapRow = 10000;
        uint16_t heatmapCol = 10000;

        // Check that heatmap and heatmapCounter are empty
        for (int x = 0; x < heatmapMultipleRuns.sizeX(); x++) {
            for (int y = 0; y < heatmapMultipleRuns.sizeY(); y++) {
                assert(heatmapMultipleRuns.at(x, y) == 0);
                assert(heatmapMultipleRunsMice.at(x, y) == 0);
                assert(heatmapMultipleRunsCats.at(x, y) == 0);
                assert(heatmapCounterMultipleRuns.at(x, y) == 0);
                assert(heatmapCounterMultipleRunsMice.at(x, y) == 0);
                assert(heatmapCounterMultipleRunsCats.at(x, y) == 0);
                assert(heatmapSumMultipleRuns.at(x, y) == 0);
                assert(heatmapSumMultipleRunsMice.at(x, y) == 0);
                assert(heatmapSumMultipleRunsCats.at(x, y) == 0);
            }
        }

        // Calculate the weights in the fields
        for (uint16_t index = 1; index <= p.population; ++index) {

            const Indiv &indiv = peeps[index];

            if (index <= p.population * p.miceRatio) {
                assert(peeps[index].species == "mouse");
            }
            if (index > p.population * p.miceRatio) {
                assert(peeps[index].species == "cat");
            }

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
                const int previousValue = heatmapMultipleRuns.at(heatmapRow, heatmapCol);
                const int previousCount = heatmapCounterMultipleRuns.at(heatmapRow, heatmapCol);
                // Set heatmap to the new value
                heatmapMultipleRuns.set(heatmapRow, heatmapCol, previousValue + conn.weight);
                heatmapCounterMultipleRuns.set(heatmapRow, heatmapCol, previousCount + 1);
                heatmapSumMultipleRuns.set(heatmapRow, heatmapCol, previousValue + conn.weight);

                if (indiv.species == "mouse") {
                    const int previousMiceValue = heatmapMultipleRunsMice.at(heatmapRow, heatmapCol);
                    const int previousMiceCount = heatmapCounterMultipleRunsMice.at(heatmapRow, heatmapCol);
                    heatmapMultipleRunsMice.set(heatmapRow, heatmapCol, previousMiceValue + conn.weight);
                    heatmapCounterMultipleRunsMice.set(heatmapRow, heatmapCol, previousMiceCount + 1);
                    heatmapSumMultipleRunsMice.set(heatmapRow, heatmapCol, previousMiceValue + conn.weight);
                } else {
                    const int previousCatValue = heatmapMultipleRunsCats.at(heatmapRow, heatmapCol);
                    const int previousCatCount = heatmapCounterMultipleRunsCats.at(heatmapRow, heatmapCol);
                    heatmapMultipleRunsCats.set(heatmapRow, heatmapCol, previousCatValue + conn.weight);
                    heatmapCounterMultipleRunsCats.set(heatmapRow, heatmapCol, previousCatCount + 1);
                    heatmapSumMultipleRunsCats.set(heatmapRow, heatmapCol, previousCatValue + conn.weight);
                }
            }
        }

        // Calculate the average of each heatmap cell
        // and also divide each cell of heatmapSum with the absolute maximum sum of weights
        for (int x = 0; x < heatmapMultipleRuns.sizeX(); x++) {
            for (int y = 0; y < heatmapMultipleRuns.sizeY(); y++) {
                if (heatmapCounterMultipleRuns.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int average = heatmapMultipleRuns.at(x, y) / heatmapCounterMultipleRuns.at(x, y);
                    heatmapMultipleRuns.set(x, y, average);
                }
                if (heatmapCounterMultipleRunsMice.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int averageMice = heatmapMultipleRunsMice.at(x, y) / heatmapCounterMultipleRunsMice.at(x, y);
                    heatmapMultipleRunsMice.set(x, y, averageMice);
                }
                if (heatmapCounterMultipleRunsCats.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int averageCats = heatmapMultipleRunsCats.at(x, y) / heatmapCounterMultipleRunsCats.at(x, y);
                    heatmapMultipleRunsCats.set(x, y, averageCats);
                }
            }
        }
    }


    // Draws the images for a specific heatmap
    void drawImageMultipleRuns(const unsigned run, const HeatmapVector& hm, const int heatmapImageScale, const std::string &type, const std::string &imageName) {
        using namespace cimg_library;

        CImg<uint8_t> image((hm.sizeX() + 6) * heatmapImageScale,
            (hm.sizeY() + 10) * heatmapImageScale,
            1,
            3,
            255);

        std::stringstream imageFilename;
        imageFilename << p.heatmapDir << "/" << imageName << "-"
        << std::setfill('0') << std::setw(6) << run
        << ".png";




        // Display the strings for sensors and internal neurons

        // First create an image which only contains the text
        CImg<uint8_t> textImage(9 * heatmapImageScale,
            hm.sizeX() * heatmapImageScale,
            1,
            3,
            255);

        // Then we add the text for the sensor and internal neurons to it
        for (int x = 0; x < hm.sizeX(); ++x) {
            if (x < NUM_SENSES) {
                textImage.draw_text(
                    0,
                    x * heatmapImageScale,
                    sensorName(static_cast<Sensor>(x)).c_str(),
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            } else {
                textImage.draw_text(
                    0,
                    x * heatmapImageScale,
                    ("N" + std::to_string(x - NUM_SENSES)).c_str(),
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            }
        }

        // Lastly we add our rotated text image to our main image
        image.draw_image(heatmapImageScale * 6, 0, textImage.get_rotate(-90));


        // Display the strings for actions and internal neurons
        for (int y = 0; y < hm.sizeY(); ++y) {
            if (y < p.maxNumberNeurons) {
                image.draw_text(
                    0,
                    (y + 10) * heatmapImageScale,
                    ("N" + std::to_string(y)).c_str(),
                    colorBlack,
                    colorWhite,
                    1.0,
                    heatmapImageScale / 2);
            } else {
                image.draw_text(
                    0,
                    (y + 10) * heatmapImageScale,
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
                            (x+6) * heatmapImageScale,
                            (y+10) * heatmapImageScale,
                            heatmapImageScale / 2,
                            colorGreen,
                            static_cast<double>(hm.at(x, y)) / 32767);
                    } else if (hm.at(x, y) < 0) {
                        image.draw_circle(
                            (x+6) * heatmapImageScale,
                            (y+10) * heatmapImageScale,
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
            for (int x = 0; x < heatmapMultipleRuns.sizeX(); x++) {
                for (int y = 0; y < heatmapMultipleRuns.sizeY(); y++) {
                    if (hm.at(x, y) > 0) {
                        image.draw_circle(
                            (x+6) * heatmapImageScale,
                            (y+10) * heatmapImageScale,
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
                    (x+6) * heatmapImageScale,
                    (y+10) * heatmapImageScale,
                    heatmapImageScale / 2,
                    hm.at(x, y) > 0 ? colorGreen : colorRed,
                    hm.at(x, y) > 0 ? static_cast<double>(hm.at(x, y)) / absoluteMaximum : static_cast<double>(hm.at(x, y)) / -absoluteMaximum);
                }
            }
        }

        // Draw horizontal lines for better visibility
        for(int y = 0; y < heatmapMultipleRuns.sizeX(); ++y) {
            image.draw_line(
            0,
            (y + 9.5) * heatmapImageScale,
            image.width(),
            (y + 9.5) * heatmapImageScale,
            colorBlack);
        }

        // Draw vertical lines for better visibility
        for(int x = 0; x < heatmapMultipleRuns.sizeX() + 1; ++x) {
        image.draw_line(
            (x + 5.5) * heatmapImageScale,
            0,
            (x + 5.5) * heatmapImageScale,
            image.height(),
            colorBlack);
        }

        // Save the heatmap image as png on the local system.
        image.save(imageFilename.str().c_str());
    }


    // Pushes the heatmap images into their lists (the origin of coordinates
    // in CImg.h is in the upper left corner).
    void saveHeatmapImagesMultipleRuns(const unsigned run) {

        drawImageMultipleRuns(run, heatmapMultipleRuns, 32, "average", "heatmap");
        drawImageMultipleRuns(run, heatmapMultipleRunsMice, 32, "average", "heatmapMice");
        drawImageMultipleRuns(run, heatmapMultipleRunsCats, 32, "average", "heatmapCats");
        drawImageMultipleRuns(run, heatmapCounterMultipleRuns, 32, "counter", "heatmapCounter");
        drawImageMultipleRuns(run, heatmapCounterMultipleRunsMice, 32, "counter", "heatmapCounterMice");
        drawImageMultipleRuns(run, heatmapCounterMultipleRunsCats, 32, "counter", "heatmapCounterCats");
        drawImageMultipleRuns(run, heatmapSumMultipleRuns, 32, "sum", "heatmapSum");
        drawImageMultipleRuns(run, heatmapSumMultipleRunsMice, 32, "sum", "heatmapSumMice");
        drawImageMultipleRuns(run, heatmapSumMultipleRunsCats, 32, "sum", "heatmapSumCats");

        // Clear the heatmap and heatmap counter after saving an image
        heatmapMultipleRuns.zeroFill();
        heatmapMultipleRunsMice.zeroFill();
        heatmapMultipleRunsCats.zeroFill();
        heatmapCounterMultipleRuns.zeroFill();
        heatmapCounterMultipleRunsMice.zeroFill();
        heatmapCounterMultipleRunsCats.zeroFill();
        heatmapSumMultipleRuns.zeroFill();
        heatmapSumMultipleRunsMice.zeroFill();
        heatmapSumMultipleRunsCats.zeroFill();
    }

}
