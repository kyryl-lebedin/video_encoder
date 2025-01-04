#ifndef FILMMASTER2000_HPP
#define FILMMASTER2000_HPP

#include <string>  // For std::string
#include <vector>  // For std::vector

using namespace std;

// Define the Video structure
struct Video1 {
    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    // memory
    std::vector<unsigned char> data;
};

struct Video2 {
    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    // frames
    vector<vector<unsigned char>> data;
};

struct Video3 {
    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    // channels
    vector<vector<vector<unsigned char>>> data;
};

// Function to read a binary input file and populate the Video struct
bool readbin1(const std::string& filename, Video1& video);

bool readbin2(const std::string& filename, Video2& video);

bool readbin3(const std::string& filename, Video3& video);

bool reverseVid(const std::string& input, const std::string& output, const std::string& mode);

bool writebin1(const std::string& filename, const Video1& video);



#endif
