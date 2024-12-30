#include "FilmMaster2000.hpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Function to read the binary input file and store into the Video struct
bool readbin(const string& filename, Video& video)
{
    // Create an ifstream object named inFile and open the file specified by filename in binary mode
    ifstream binVideo(filename, ios::binary);
    if (!binVideo.is_open()) {
        cerr << "Error: Failed to open file " << filename << endl;
        return false; // Return false if file cannot be opened
    }

    // Read the number of frames from the binary file and store into video.noFrames
    binVideo.read(reinterpret_cast<char*>(&video.noFrames), sizeof(video.noFrames));
    if (!binVideo) { // Check if read operation was successful
        cerr << "Error: Failed to read number of frames." << endl;
        return false;
    }

    binVideo.read(reinterpret_cast<char*>(&video.channels), sizeof(video.channels));
    if (!binVideo) {
        cerr << "Error: Failed to read number of channels." << endl;
        return false;
    }

    binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
    if (!binVideo) {
        cerr << "Error: Failed to read width." << endl;
        return false;
    }

    binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));
    if (!binVideo) {
        cerr << "Error: Failed to read height." << endl;
        return false;
    }

    std::cout << "Number of frames: " << video.noFrames << std::endl;
    // noFrames is an integer type, so it can be directly output without casting
    std::cout << "Number of channels: " << static_cast<int>(video.channels) << std::endl;
    // channels is likely an unsigned char or similar type, so we use static_cast<int> to convert it to an int for proper display
    std::cout << "Width: " << static_cast<int>(video.width) << std::endl;
    // width is likely an unsigned char or similar type, so we use static_cast<int> to convert it to an int for proper display
    std::cout << "Height: " << static_cast<int>(video.height) << std::endl;
    // height is likely an unsigned char or similar type, so we use static_cast<int> to convert it to an int for proper display

    // Calculate the total size needed for the data vector, convert to chat
    size_t totalSize = static_cast<size_t>(video.noFrames) * video.channels * video.width * video.height;

    // Check for overflow
    if (totalSize / video.channels / video.width / video.height != video.noFrames) {
        cerr << "Error: Overflow detected while calculating total size for data vector." << endl;
        return false;
    }

    // Prepare the data vector
    video.data.resize(totalSize);
    // Print the size of the data vector
    std::cout << "Size of data vector: " << video.data.size() << std::endl;

    // Read the pixel data into the data vector
    binVideo.read(reinterpret_cast<char*>(video.data.data()), totalSize);
    if (!binVideo) {
        cerr << "Error: Failed to read pixel data." << endl;
        return false;
    }

    binVideo.close();

    return true;
}

