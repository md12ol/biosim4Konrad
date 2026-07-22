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
    HeatmapVector heatmapMice;
    HeatmapVector heatmapCats;
    HeatmapVector heatmapCounter;
    HeatmapVector heatmapCounterMice;
    HeatmapVector heatmapCounterCats;
    HeatmapVector heatmapSum;
    HeatmapVector heatmapSumMice;
    HeatmapVector heatmapSumCats;
    cimg_library::CImgList<uint8_t> heatmapImageList;
    cimg_library::CImgList<uint8_t> heatmapImageListMice;
    cimg_library::CImgList<uint8_t> heatmapImageListCats;
    cimg_library::CImgList<uint8_t> heatmapCounterImageList;
    cimg_library::CImgList<uint8_t> heatmapCounterImageListMice;
    cimg_library::CImgList<uint8_t> heatmapCounterImageListCats;
    cimg_library::CImgList<uint8_t> heatmapSumImageList;
    cimg_library::CImgList<uint8_t> heatmapSumImageListMice;
    cimg_library::CImgList<uint8_t> heatmapSumImageListCats;

    constexpr uint8_t colorGreen[3] = {0, 255, 0};
    constexpr uint8_t colorRed[3] = {255, 0, 0};
    constexpr uint8_t colorBlack[3] = {0, 0, 0};
    constexpr uint8_t colorWhite[3] = {255, 255, 255};

    // Initializes a heatmap with size depending on Sensors, Neurons and Actions.
    void initHeatmap() {

        // Initialize the heatmap depending on the number of neurons
        std::cout << "heatmap initialized" << std::endl;
        heatmap.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapMice.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCats.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounter.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounterMice.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapCounterCats.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSum.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSumMice.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
        heatmapSumCats.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);
    }

    void fillHeatmap() {
        // Access the heatmap at the correct indices
        uint16_t heatmapRow = 10000;
        uint16_t heatmapCol = 10000;

        // Check that heatmap and heatmapCounter are empty
        for (int x = 0; x < heatmap.sizeX(); x++) {
            for (int y = 0; y < heatmap.sizeY(); y++) {
                assert(heatmap.at(x, y) == 0);
                assert(heatmapMice.at(x, y) == 0);
                assert(heatmapCats.at(x, y) == 0);
                assert(heatmapCounter.at(x, y) == 0);
                assert(heatmapCounterMice.at(x, y) == 0);
                assert(heatmapCounterCats.at(x, y) == 0);
                assert(heatmapSum.at(x, y) == 0);
                assert(heatmapSumMice.at(x, y) == 0);
                assert(heatmapSumCats.at(x, y) == 0);
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

                if (indiv.species == "mouse") {
                    const int previousMiceValue = heatmapMice.at(heatmapRow, heatmapCol);
                    const int previousMiceCount = heatmapCounterMice.at(heatmapRow, heatmapCol);
                    heatmapMice.set(heatmapRow, heatmapCol, previousMiceValue + conn.weight);
                    heatmapCounterMice.set(heatmapRow, heatmapCol, previousMiceCount + 1);
                    heatmapSumMice.set(heatmapRow, heatmapCol, previousMiceValue + conn.weight);
                } else {
                    const int previousCatValue = heatmapCats.at(heatmapRow, heatmapCol);
                    const int previousCatCount = heatmapCounterCats.at(heatmapRow, heatmapCol);
                    heatmapCats.set(heatmapRow, heatmapCol, previousCatValue + conn.weight);
                    heatmapCounterCats.set(heatmapRow, heatmapCol, previousCatCount + 1);
                    heatmapSumCats.set(heatmapRow, heatmapCol, previousCatValue + conn.weight);
                }
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
                if (heatmapCounterMice.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int averageMice = heatmapMice.at(x, y) / heatmapCounterMice.at(x, y);
                    heatmapMice.set(x, y, averageMice);
                }
                if (heatmapCounterCats.at(x, y) > 1) {
                    // Calculate the average (if there is only one value, we do not need to modify it)
                    const int averageCats = heatmapCats.at(x, y) / heatmapCounterCats.at(x, y);
                    heatmapCats.set(x, y, averageCats);
                }
            }
        }
    }

    // Draws the images for a specific heatmap
    void drawImage(const unsigned generation, const HeatmapVector& hm,
        cimg_library::CImgList<uint8_t>& heatmapList, const int heatmapImageScale, const std::string &type) {
        using namespace cimg_library;

        CImg<uint8_t> image((hm.sizeX() + 6) * heatmapImageScale,
            (hm.sizeY() + 10) * heatmapImageScale,
            1,
            3,
            255);

        std::stringstream imageFilename;
        imageFilename << p.imageDir << "frame-"
        << std::setfill('0') << std::setw(6) << generation
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
            for (int x = 0; x < heatmap.sizeX(); x++) {
                for (int y = 0; y < heatmap.sizeY(); y++) {
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
        for(int y = 0; y < heatmap.sizeX(); ++y) {
            image.draw_line(
            0,
            (y + 9.5) * heatmapImageScale,
            image.width(),
            (y + 9.5) * heatmapImageScale,
            colorBlack);
        }

        // Draw vertical lines for better visibility
        for(int x = 0; x < heatmap.sizeX() + 1; ++x) {
        image.draw_line(
            (x + 5.5) * heatmapImageScale,
            0,
            (x + 5.5) * heatmapImageScale,
            image.height(),
            colorBlack);
        }
        heatmapList.push_back(image);
    }


    // Pushes the heatmap images into their lists (the origin of coordinates
    // in CImg.h is in the upper left corner).
    void saveHeatmapImages(const unsigned generation) {

        drawImage(generation, heatmap, heatmapImageList, 32, "average");
        drawImage(generation, heatmapMice, heatmapImageListMice, 32, "average");
        drawImage(generation, heatmapCats, heatmapImageListCats, 32, "average");
        drawImage(generation, heatmapCounter, heatmapCounterImageList, 32, "counter");
        drawImage(generation, heatmapCounterMice, heatmapCounterImageListMice, 32, "counter");
        drawImage(generation, heatmapCounterCats, heatmapCounterImageListCats, 32, "counter");
        drawImage(generation, heatmapSum, heatmapSumImageList, 32, "sum");
        drawImage(generation, heatmapSumMice, heatmapSumImageListMice, 32, "sum");
        drawImage(generation, heatmapSumCats, heatmapSumImageListCats, 32, "sum");

        // Clear the heatmap and heatmap counter after saving an image
        heatmap.zeroFill();
        heatmapMice.zeroFill();
        heatmapCats.zeroFill();
        heatmapCounter.zeroFill();
        heatmapCounterMice.zeroFill();
        heatmapCounterCats.zeroFill();
        heatmapSum.zeroFill();
        heatmapSumMice.zeroFill();
        heatmapSumCats.zeroFill();
    }

    // Takes a list of heatmap images and creates a video with one frame for each generation.
    void createSingleHeatmapVideo(cimg_library::CImgList<uint8_t>& heatmapList, const std::string &videoName, unsigned generation) {
        if (heatmapList.size() > 0) {
            std::cout << "Frames are in the " << videoName << std::endl;
            std::cout << "Number of frames in the " << videoName << ": " << heatmapList.size() << std::endl;
            std::stringstream videoFilename;
            videoFilename << p.heatmapDir.c_str() << "/" << videoName << "-"
            << std::setfill('0') << std::setw(6) << generation << ".mp4";
            heatmapList.save_video(videoFilename.str().c_str(),
            1,
            "mp4v",
            false);
        }
        heatmapList.clear();
    }

    // Takes the list of all heatmap images and creates a video with one frame for each generation.
    void createHeatmapVideos(unsigned generation) {

        createSingleHeatmapVideo(heatmapImageList, "heatmap", generation);
        createSingleHeatmapVideo(heatmapImageListMice, "heatmapMice", generation);
        createSingleHeatmapVideo(heatmapImageListCats, "heatmapCats", generation);
        createSingleHeatmapVideo(heatmapCounterImageList, "heatmapCounter", generation);
        createSingleHeatmapVideo(heatmapCounterImageListMice, "heatmapCounterMice", generation);
        createSingleHeatmapVideo(heatmapCounterImageListCats, "heatmapCounterCats", generation);
        createSingleHeatmapVideo(heatmapSumImageList, "heatmapSum", generation);
        createSingleHeatmapVideo(heatmapSumImageListMice, "heatmapSumMice", generation);
        createSingleHeatmapVideo(heatmapSumImageListCats, "heatmapSumCats", generation);
    }
}
