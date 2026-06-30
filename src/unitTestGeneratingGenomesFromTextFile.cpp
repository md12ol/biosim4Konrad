// unitTestGeneratingGenomesFromTextFile.cpp
// This reads genomes from a textfile and writes the readed genomes into a second textfile to test their equality.
// See analysis.cpp and spawnNewGeneration.cpp to see how genomes are written and read to/from textfiles respectively.

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>
#include "simulator.h"

namespace BS {

    bool unitTestGeneratingGenomesFromTextFile() {
        std::ifstream finputGenomes("./genomes/genome-mice-0-test.txt");
        std::vector<std::string> genomes;

        if (finputGenomes.is_open()) {
            std::string genome;
            // Read the genomes from the textfile
            while (std::getline(finputGenomes, genome)) {
                genomes.push_back(genome);
            }
        } else {
            std::cerr << "Error opening file" << std::endl;
            return false;
        }

        finputGenomes.close();

        std::vector<Indiv> testIndividuals;

        // Put test individuals into the container, with the genomes from the textfile.
        for (int i = 0; i < genomes.size(); i++) {
            Indiv indiv;
            std::string line = genomes[i];

            // Delete the individual index.
            std::size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                line.erase(0, pos + 1);
            }

            // Generate genome from string, each hex value being a gene.
            std::stringstream stream(line);
            Genome indivGenome;
            std::string geneHexValue;
            while (stream >> geneHexValue) {
                Gene gene;
                uint32_t n = static_cast<uint32_t>(std::stoul(geneHexValue, nullptr, 16));
                std::memcpy(&gene, &n, sizeof(gene));
                indivGenome.push_back(gene);
            }

            // Set the genome of the indiv to the created genome.
            indiv.genome = indivGenome;

            // And push it to the container with the test individuals.
            testIndividuals.push_back(indiv);
        }

        // Now convert the individual genomes to hex strings again.
        std::ofstream foutputGenomes("./genomes/genome-mice-test-output.txt");

        for (int i = 0; i < testIndividuals.size(); i++) {
            if (foutputGenomes.is_open()) {
                foutputGenomes << std::dec << (i+1);
                for (Gene gene : testIndividuals[i].genome) {
                    uint32_t n;
                    std::memcpy(&n, &gene, sizeof(n));
                    foutputGenomes << " " << std::hex << std::setfill('0') << std::setw(8) << n;
                }
                foutputGenomes << std::endl;

            } else {
                std::cerr << "Error opening file" << std::endl;
                return false;
            }
        }

        foutputGenomes.close();
        std::ifstream finputGenomes2("./genomes/genome-mice-test2.txt");
        std::vector<std::string> genomes2;
        std::string genome2;

        // Now check if the hex genomes are the same.
        for (int i = 0; i < genomes2.size(); i++) {
            if (genomes.at(i) != genomes2.at(i)) {
                return false;
            }
        }
        return true;
    }
}
