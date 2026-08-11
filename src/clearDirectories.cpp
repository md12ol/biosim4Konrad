#include "clearDirectories.h"

#include "simulator.h"
#include <iostream>
#include <filesystem>

namespace BS {
    void clearDirectories() {
        std::filesystem::path genomeDir = "output/" + p.genomeDir;
        std::filesystem::path graphDir = "output/" + p.graphDir;
        std::filesystem::path heatmapDir = "output/" + p.heatmapDir;
        std::filesystem::path imageDir = "output/" + p.imageDir;
        std::filesystem::path netDir = "output/" + p.netDir;

        if (p.numRuns == 1) {
            for (const auto& entry : std::filesystem::directory_iterator(genomeDir)) {
                std::filesystem::remove_all(entry.path());
            }
        }

        for (const auto& entry : std::filesystem::directory_iterator(graphDir)) {
            std::filesystem::remove_all(entry.path());
        }

        for (const auto& entry : std::filesystem::directory_iterator(heatmapDir)) {
            std::filesystem::remove_all(entry.path());
        }

        for (const auto& entry : std::filesystem::directory_iterator(imageDir)) {
            std::filesystem::remove_all(entry.path());
        }

        for (const auto& entry : std::filesystem::directory_iterator(netDir)) {
            std::filesystem::remove_all(entry.path());
        }
    }
}

