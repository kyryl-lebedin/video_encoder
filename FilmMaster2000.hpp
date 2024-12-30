#ifndef FILMMASTER2000_HPP
#define FILMMASTER2000_HPP

#include <string>  // For std::string
#include <vector>  // For std::vector

// Define the Video structure
struct Video {
    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    // Storing pixel data
    std::vector<unsigned char> data;
};

// Function to read a binary input file and populate the Video struct
bool readbin(const std::string& filename, Video& video);



#endif
