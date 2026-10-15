// postProcess.cpp
//
// Rebuilds the outputs that only depend on data the simulation already writes
// to disk, so that none of them have to be produced while evolution is running:
//
//   - heatmaps and the heatmap videos (from the genome dumps)
//   - neural net edge lists and their graph pictures (from the genome dumps)
//   - the gnuplot survival graph (from the epoch log)
//
// The genome dumps written by displaySampleGenomes() contain the complete
// population of a generation, and the wiring of a neural net is a deterministic
// function of the genome, so every one of these outputs can be recreated
// exactly after the run has finished.
//
// Usage:  ./biosim4 <config.ini> --postprocess
//
// The config file must be the one the run was made with, because the output
// path and the network parameters (e.g. maxNumberNeurons) are taken from it.

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "simulator.h"
#include "heatmapWriter.h"

namespace BS {

extern Genome generateGenomeFromVector(const std::vector<std::string> &lines, uint16_t index);

namespace {

std::vector<std::string> readLines(const std::string &filename)
{
    std::vector<std::string> lines;
    std::ifstream f(filename);
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    return lines;
}

// The first token of a genome dump line is the index the individual had in peeps[].
unsigned originalIndexOf(const std::string &dumpLine)
{
    return static_cast<unsigned>(std::stoul(dumpLine.substr(0, dumpLine.find(' '))));
}

// Returns, in ascending order, every generation that has both a mice and a
// cats genome dump in the genome directory.
std::vector<unsigned> dumpedGenerations(const std::string &genomeDir)
{
    const std::string micePrefix = "genome-mice-";
    const std::string suffix = ".txt";
    std::vector<unsigned> generations;

    for (const auto &entry : std::filesystem::directory_iterator(genomeDir)) {
        const std::string name = entry.path().filename().string();
        if (name.rfind(micePrefix, 0) != 0 || name.size() <= micePrefix.size() + suffix.size()
                || name.compare(name.size() - suffix.size(), suffix.size(), suffix) != 0) {
            continue;
        }
        const std::string number = name.substr(micePrefix.size(), name.size() - micePrefix.size() - suffix.size());
        if (number.find_first_not_of("0123456789") != std::string::npos) {
            continue;
        }
        const unsigned generation = static_cast<unsigned>(std::stoul(number));
        if (std::filesystem::exists(genomeDir + "/genome-cats-" + number + ".txt")) {
            generations.push_back(generation);
        }
    }
    std::sort(generations.begin(), generations.end());
    return generations;
}

// Reads the list of individuals whose nets should be drawn for a generation.
// The file is written by displaySampleGenomes() and has one "species index"
// pair per line; it may legitimately be empty. Returns false if the file does
// not exist (dumps made before this file was introduced).
bool readSampleIndexes(const std::string &filename, std::vector<std::pair<std::string, unsigned>> &samples)
{
    std::ifstream f(filename);
    if (!f.is_open()) {
        return false;
    }
    std::string species;
    unsigned index;
    while (f >> species >> index) {
        samples.emplace_back(species, index);
    }
    return true;
}

} // end anonymous namespace


void postProcess()
{
    paramManager.updateFromConfigFile(0);
    paramManager.checkParameters();
    paramManager.setOutputPath();
    randomUint.initialize();

    if (p.numRuns != 1) {
        std::cout << "Post-processing needs the genome dumps of a single-run simulation (numRuns = 1)." << std::endl;
        return;
    }

    const std::string genomeDir = p.outputPath + p.genomeDir;
    if (!std::filesystem::is_directory(genomeDir)) {
        std::cout << "No genome directory found at " << genomeDir << std::endl;
        return;
    }
    const std::vector<unsigned> generations = dumpedGenerations(genomeDir);
    if (generations.empty()) {
        std::cout << "No genome dumps found in " << genomeDir << std::endl;
        return;
    }

    initHeatmap();
    grid.init(p.sizeX, p.sizeY); // Indiv::initialize() registers every individual in the grid

    for (unsigned generation : generations) {
        // Pick up any parameter@generation overrides that were active at this generation.
        paramManager.updateFromConfigFile(generation);

        const std::vector<std::string> mice = readLines(genomeDir + "/genome-mice-" + std::to_string(generation) + ".txt");
        const std::vector<std::string> cats = readLines(genomeDir + "/genome-cats-" + std::to_string(generation) + ".txt");
        const unsigned population = mice.size() + cats.size();
        if (population == 0) {
            std::cout << "Generation " << generation << ": empty genome dump, skipped" << std::endl;
            continue;
        }

        // The population may have changed during the run (dynamicPopulation),
        // so size everything from the dump rather than from the config file.
        // The ratio is computed exactly as spawnNewGeneration() does, because
        // fillHeatmap() and the heatmap drawing use population * miceRatio.
        p.population = population;
        p.miceRatio = static_cast<double>(mice.size()) / population;
        peeps.init(population);
        grid.zeroFill();

        // Rebuild the individuals in dump order: mice first, then cats. The
        // indexes they had during the run are kept for naming the net files.
        std::vector<unsigned> originalIndex(population + 1, 0);
        std::map<unsigned, uint16_t> slotOfOriginal;
        uint16_t slot = 1;
        for (size_t i = 0; i < mice.size(); ++i, ++slot) {
            peeps[slot].initialize(slot, Coord(0, 0), generateGenomeFromVector(mice, i), "mouse");
            originalIndex[slot] = originalIndexOf(mice[i]);
            slotOfOriginal[originalIndex[slot]] = slot;
        }
        for (size_t i = 0; i < cats.size(); ++i, ++slot) {
            peeps[slot].initialize(slot, Coord(0, 0), generateGenomeFromVector(cats, i), "cat");
            originalIndex[slot] = originalIndexOf(cats[i]);
            slotOfOriginal[originalIndex[slot]] = slot;
        }

        // Heatmaps: one set of images per dumped generation, collected into videos below.
        fillHeatmap();
        saveHeatmapImages(0, generation);

        // Neural nets: the individuals the run selected (best scored, if
        // displayBestGenomes was set). Dumps without a sample file fall back
        // to the first few of each species.
        std::vector<std::pair<std::string, unsigned>> samples;
        if (!readSampleIndexes(genomeDir + "/sample-indexes-" + std::to_string(generation) + ".txt", samples)) {
            for (size_t i = 0; i < std::min<size_t>(p.displaySampleGenomes, mice.size()); ++i) {
                samples.emplace_back("mouse", originalIndex[1 + i]);
            }
            for (size_t i = 0; i < std::min<size_t>(p.displaySampleGenomes, cats.size()); ++i) {
                samples.emplace_back("cat", originalIndex[1 + mice.size() + i]);
            }
        }
        for (const auto &[species, index] : samples) {
            const auto found = slotOfOriginal.find(index);
            if (found == slotOfOriginal.end() || peeps[found->second].species != species) {
                std::cout << "Generation " << generation << ": " << species << " " << index
                          << " is not in the genome dump, net skipped" << std::endl;
                continue;
            }
            peeps[found->second].printIGraphEdgeList(index, generation, species);
        }

        std::cout << "Post-processed generation " << generation << ": "
                  << mice.size() << " mice, " << cats.size() << " cats, "
                  << samples.size() << " nets" << std::endl;
    }

    createHeatmapVideos(generations.back());

    // The survival graph only needs the epoch log.
    const std::string command = p.graphLogFinalUpdateCommand + " " + p.outputPath + " " + p.imageDir + " " + p.logDir;
    if (std::system(command.c_str()) != 0) {
        std::cout << "gnuplot command failed: " << command << std::endl;
    }

    std::cout << "Post-processing done." << std::endl;
}

} // end namespace BS
