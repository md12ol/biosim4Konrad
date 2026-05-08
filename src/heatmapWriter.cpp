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

namespace BS {

    void createHeatmap() {
        HeatmapVector heatmap;

        // Initialize the heatmap depending on the number of neurons
        heatmap.init(NUM_SENSES + p.maxNumberNeurons, NUM_ACTIONS + p.maxNumberNeurons);

        // Access the heatmap at the correct indices
        uint16_t heatmapRow = -1;
        uint16_t heatmapCol = -1;

        // Calculate the weights in the fields
        for (uint16_t index = 1; index <= p.population; index++) {
            const Indiv &indiv = peeps[index];
            if (index == 1) {
                // Check the neuronal net of an indiv
                for (auto &conn : indiv.nnet.connections) {
                    // Check the source (row) of the connection
                    if (conn.sourceType == SENSOR) {
                        heatmapRow = (Sensor)conn.sourceNum;
                        std::cout << "Sensor should be below 53: " << heatmapRow << std::endl;
                    } else if (conn.sourceType == NEURON) {
                        std::cout << "Neuron should be below 5: " << conn.sourceNum << std::endl;
                        heatmapRow = NUM_SENSES + conn.sourceNum;
                    }

                    // Check the sink (column) of the connection
                    if (conn.sinkType == ACTION) {
                        std::cout << "Action should be below 17: " << (Action)conn.sinkNum << std::endl;
                        // Display the Actions after the neurons in the columns
                        heatmapCol = (Action)conn.sinkNum + p.maxNumberNeurons;
                    } else if (conn.sinkType == NEURON) {
                        std::cout << "Neuron should be below 5: " << conn.sinkNum << std::endl;
                        heatmapCol = conn.sourceNum;
                    }

                    // Get the previous value for the connection in the heatmap
                    int previousValue = heatmap.at(heatmapRow, heatmapCol);
                    // Set heatmap to the new value
                    heatmap.set(heatmapRow, heatmapCol, previousValue + conn.weight);
                    // Print out weight as a test
                    std::cout << "Weight of a connection: " << heatmap.at(heatmapRow, heatmapCol) << std::endl;
                }
            }
        }
    }
}
