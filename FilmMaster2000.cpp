#include "FilmMaster2000.hpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


// Function to read the binary input file and store into the Video struct
bool readbin(const string& filename, Video& video)
{
    // Create an ifstream object named inFile and open the file specified by filename in binary mode
    ifstream binvideo(filename, ios::binary);
    if (!binvideo.is_open()) {
        cerr << "Error: Failed to open file " << filename << endl;
        return false; // Return false if file cannot be opened
    }

    // Read the number of frames from the binary file into video.noFrames
    binvideo.read(reinterpret_cast<char*>(&video.noFrames), sizeof(video.noFrames));
    if (!binvideo) { // Check if read operation was successful
        cerr << "Error: Failed to read number of frames." << endl;
        return false;
    }

    std::cout << "Number of frames: " << video.noFrames << std::endl;

    return true;
}

