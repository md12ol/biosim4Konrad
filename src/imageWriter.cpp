// imageWriter.cpp

#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "simulator.h"
#include "imageWriter.h"
#define cimg_use_opencv 1
#define cimg_display 0
#include "CImg.h"

namespace BS {

struct Array3 {
    double rgb[3];
};


/**
 * Taken from Earl F. Glynn's web page:
 * <a href="http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm">Spectra Lab Report</a>
 */
Array3 waveLengthToRGB(double Wavelength) {
    constexpr double Gamma = 0.80;
    constexpr double IntensityMax = 255;
    double factor;
    double Red, Green, Blue;

    if((Wavelength >= 380) && (Wavelength < 440)) {
        Red = -(Wavelength - 440) / (440 - 380);
        Green = 0.0;
        Blue = 1.0;
    } else if((Wavelength >= 440) && (Wavelength < 490)) {
        Red = 0.0;
        Green = (Wavelength - 440) / (490 - 440);
        Blue = 1.0;
    } else if((Wavelength >= 490) && (Wavelength < 510)) {
        Red = 0.0;
        Green = 1.0;
        Blue = -(Wavelength - 510) / (510 - 490);
    } else if((Wavelength >= 510) && (Wavelength < 580)) {
        Red = (Wavelength - 510) / (580 - 510);
        Green = 1.0;
        Blue = 0.0;
    } else if((Wavelength >= 580) && (Wavelength < 645)) {
        Red = 1.0;
        Green = -(Wavelength - 645) / (645 - 580);
        Blue = 0.0;
    } else if((Wavelength >= 645) && (Wavelength < 781)) {
        Red = 1.0;
        Green = 0.0;
        Blue = 0.0;
    } else {
        Red = 0.0;
        Green = 0.0;
        Blue = 0.0;
    }

    // Let the intensity fall off near the vision limits

    if((Wavelength >= 380) && (Wavelength < 420)) {
        factor = 0.3 + 0.7 * (Wavelength - 380) / (420 - 380);
    } else if((Wavelength >= 420) && (Wavelength < 701)) {
        factor = 1.0;
    } else if((Wavelength >= 701) && (Wavelength < 781)) {
        factor = 0.3 + 0.7 * (780 - Wavelength) / (780 - 700);
    } else {
        factor = 0.0;
    }


    Array3 rgb;

    // Don't want 0^x = 1 for x <> 0
    rgb.rgb[0] = Red == 0.0 ? 0 : (int)std::round(IntensityMax * std::pow(Red * factor, Gamma));
    rgb.rgb[1] = Green == 0.0 ? 0 : (int)std::round(IntensityMax * std::pow(Green * factor, Gamma));
    rgb.rgb[2] = Blue == 0.0 ? 0 : (int)std::round(IntensityMax * std::pow(Blue * factor, Gamma));

    if (rgb.rgb[0] <= 0 && rgb.rgb[1] <= 0 && rgb.rgb[2] <= 0) {
        std::cerr << "RGB values must be positive(wavelengthToRGB)." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return rgb;
}

cimg_library::CImgList<uint8_t> imageList;

// Pushes a new image frame onto .imageList.
//


void saveOneFrameImmed(const ImageFrameData &data)
{
    using namespace cimg_library;

    CImg<uint8_t> image(p.sizeX * p.displayScale, p.sizeY * p.displayScale,
                        1,   // Z depth
                        3,   // color channels
                        255);  // initial value
    uint8_t color[3];
    std::stringstream imageFilename;
    imageFilename << p.imageDir << "frame-"
                  << std::setfill('0') << std::setw(6) << data.generation
                  << '-' << std::setfill('0') << std::setw(6) << data.simStep
                  << ".png";

    // Draw food locations

    if (p.displayFoodLocations == true) {
        color[0] = 255;
        color[1] = 255;
        color[2] = 0;
        image.draw_circle(
                (p.sizeX * p.displayScale) / 2.0,
                (p.sizeY * p.displayScale) / 2.0,
                (p.sizeX * p.displayScale) / 3.0,
                color,
                1.0
            );
    }

    // Draw barrier locations

    color[0] = color[1] = color[2] = 0x88;
    for (Coord loc : data.barrierLocs) {
            image.draw_rectangle(
                loc.x       * p.displayScale - (p.displayScale / 2), ((p.sizeY - loc.y) - 1)   * p.displayScale - (p.displayScale / 2),
                (loc.x + 1) * p.displayScale, ((p.sizeY - (loc.y - 0))) * p.displayScale,
                color,  // rgb
                1.0);  // alpha
    }

    // Draw safeAreaLocations

    if (p.displaySafeAreas == true) {
        color[0] = 0;
        color[1] = 255;
        color[2] = 0;
        for (Coord loc : data.safeAreaLocs) {
            image.draw_rectangle(
                loc.x       * p.displayScale - (p.displayScale / 2), ((p.sizeY - loc.y) - 1)   * p.displayScale - (p.displayScale / 2),
                (loc.x + 1) * p.displayScale, ((p.sizeY - (loc.y - 0))) * p.displayScale,
                color,
                1.0);  // alpha
        }
    }

    // Draw agents

    for (size_t i = 0; i < data.indivLocs.size(); ++i) {
            Array3 colors = waveLengthToRGB(data.indivColors[i]);
            color[0] = (uint8_t)colors.rgb[0];
            color[1] = (uint8_t)colors.rgb[1];
            color[2] = (uint8_t)colors.rgb[2];
            if (color[0] == 0 && color[1] == 0 && color[2] == 0) {
                std::cerr << "RGB values must be positive(saveOneFrameImmed)." << std::endl;
                std::exit(EXIT_FAILURE);
            }

        image.draw_circle(
                data.indivLocs[i].x * p.displayScale,
                ((p.sizeY - data.indivLocs[i].y) - 1) * p.displayScale,
                p.agentSize,
                color,  // rgb
                1.0);  // alpha
    }

    //image.save_png(imageFilename.str().c_str(), 3);
    imageList.push_back(image);

    //CImgDisplay local(image, "biosim3");
}


// Starts the image writer asynchronous thread.
ImageWriter::ImageWriter()
    : droppedFrameCount{0}, busy{true}, dataReady{false},
      abortRequested{false}
{
    startNewGeneration();
}


void ImageWriter::startNewGeneration()
{
    imageList.clear();
    skippedFrames = 0;
}


double makeGeneticColor(const Genome &genome, const std::string &species)
{
    double val = ((genome.size() & 1)
         | ((genome.front().sourceType)    << 1)
         | ((genome.back().sourceType)     << 2)
         | ((genome.front().sinkType)      << 3)
         | ((genome.back().sinkType)       << 4)
         | ((genome.front().sourceNum & 1) << 5)
         | ((genome.front().sinkNum & 1)   << 6));
         //| ((genome.back().sourceNum & 1)  << 7));


    if (species == "mouse") {
        if (400 + val / 2 < 0) {
            std::cerr << "Value should not be below 0(makeGeneticColor).";
            std::exit(EXIT_FAILURE);
        }
        return 400 + val / 2;
    } else {
        if (610 + val / 2 < 0) {
            std::cerr << "Value should not be below 0(makeGeneticColor).";
            std::exit(EXIT_FAILURE);
        }
        return 610 + val / 2;
    }
}


// This is a synchronous gate for giving a job to saveFrameThread().
// Called from the same thread as the main simulator loop thread during
// single-thread mode.
// Returns true if the image writer accepts the job; returns false
// if the image writer is busy. Always called from a single thread
// and communicates with a single saveFrameThread(), so no need to make
// a critical section to safeguard the busy flag. When this function
// sets the busy flag, the caller will immediate see it, so the caller
// won't call again until busy is clear. When the thread clears the busy
// flag, it doesn't matter if it's not immediately visible to this
// function: there's no consequence other than a harmless frame-drop.
// The condition variable allows the saveFrameThread() to wait until
// there's a job to do.
bool ImageWriter::saveVideoFrame(unsigned simStep, unsigned generation)
{
    if (!busy) {
        busy = true;
        // queue job for saveFrameThread()
        // We cache a local copy of data from params, grid, and peeps because
        // those objects will change by the main thread at the same time our
        // saveFrameThread() is using it to output a video frame.
        data.simStep = simStep;
        data.generation = generation;
        data.indivLocs.clear();
        data.indivColors.clear();
        data.barrierLocs.clear();
        data.safeAreaLocs.clear();
        data.signalLayers.clear();
        //todo!!!
        for (uint16_t index = 1; index <= p.population; ++index) {
            const Indiv &indiv = peeps[index];
            if (indiv.alive) {
                data.indivLocs.push_back(indiv.loc);
                data.indivColors.push_back(makeGeneticColor(indiv.genome, indiv.species));
            }
        }

        auto const &barrierLocs = grid.getBarrierLocations();
        for (Coord loc : barrierLocs) {
            data.barrierLocs.push_back(loc);
        }

        auto const &safeAreaLocs = grid.getSafeAreaLocations();
        for (Coord loc : safeAreaLocs) {
            data.safeAreaLocs.push_back(loc);
        }

        // tell thread there's a job to do
        {
            std::lock_guard<std::mutex> lck(mutex_);
            dataReady = true;
        }
        condVar.notify_one();
        return true;
    } else {
        // image saver thread is busy, drop a frame
        ++droppedFrameCount;
        return false;
    }
}


// Synchronous version, always returns true
bool ImageWriter::saveVideoFrameSync(unsigned simStep, unsigned generation)
{
    // We cache a local copy of data from params, grid, and peeps because
    // those objects will change by the main thread at the same time our
    // saveFrameThread() is using it to output a video frame.
    data.simStep = simStep;
    data.generation = generation;
    data.indivLocs.clear();
    data.indivColors.clear();
    data.barrierLocs.clear();
    data.safeAreaLocs.clear();
    data.signalLayers.clear();
    //todo!!!
    for (uint16_t index = 1; index <= p.population; ++index) {
        const Indiv &indiv = peeps[index];
        if (indiv.alive) {
            data.indivLocs.push_back(indiv.loc);
            data.indivColors.push_back(makeGeneticColor(indiv.genome, indiv.species));
        }
    }

    auto const &barrierLocs = grid.getBarrierLocations();
    for (Coord loc : barrierLocs) {
        data.barrierLocs.push_back(loc);
    }

    auto const &safeAreaLocs = grid.getSafeAreaLocations();
    for (Coord loc : safeAreaLocs) {
        data.safeAreaLocs.push_back(loc);
    }

    saveOneFrameImmed(data);
    return true;
}


// ToDo: put save_video() in its own thread
void ImageWriter::saveGenerationVideo(unsigned generation)
{
    if (imageList.size() > 0) {
        std::stringstream videoFilename;
        videoFilename << p.imageDir.c_str() << "/gen-"
                      << std::setfill('0') << std::setw(6) << generation
                      << ".avi";
        cv::setNumThreads(2);
        imageList.save_video(videoFilename.str().c_str(),
                             25,
                             "H264");
        if (skippedFrames > 0) {
            std::cout << "Video skipped " << skippedFrames << " frames" << std::endl;
        }
    }
    startNewGeneration();
}


void ImageWriter::abort()
{
    busy =true;
    abortRequested = true;
    {
        std::lock_guard<std::mutex> lck(mutex_);
        dataReady = true;
    }
    condVar.notify_one();
}


// Runs in a thread; wakes up when there's a video frame to generate.
// When this wakes up, local copies of Params and Peeps will have been
// cached for us to use.
void ImageWriter::saveFrameThread()
{
    busy = false; // we're ready for business
    std::cout << "Imagewriter thread started." << std::endl;

    while (true) {
        // wait for job on queue
        std::unique_lock<std::mutex> lck(mutex_);
        condVar.wait(lck, [&]{ return dataReady && busy; });
        // save frame
        dataReady = false;
        busy = false;

        if (abortRequested) {
            break;
        }

        // save image frame
        saveOneFrameImmed(imageWriter.data);

        //std::cout << "Image writer thread waiting..." << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(2));

    }
    std::cout << "Image writer thread exiting." << std::endl;
}

} // end namespace BS
