#include "FilmMaster2000.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


// Function to read the binary input file and store into the Video struct
bool readbin1(const string& filename, Video1& video)
{
    // Create an ifstream object named inFile and open the file specified by filename in binary mode
    ifstream binVideo(filename, ios::binary);
    if (!binVideo.is_open()) {
        cerr << "Error: Failed to open file " << filename << endl;
        return false; // Return false if file cannot be opened
    }

    
    binVideo.read(reinterpret_cast<char*>(&video.noFrames), sizeof(video.noFrames));
    binVideo.read(reinterpret_cast<char*>(&video.channels), sizeof(video.channels));
    binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
    binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));
    

    std::cout << "Number of frames: " << video.noFrames << std::endl;
    std::cout << "Number of channels: " << static_cast<int>(video.channels) << std::endl;
    std::cout << "Width: " << static_cast<int>(video.width) << std::endl;
    std::cout << "Height: " << static_cast<int>(video.height) << std::endl;
    

    // Calculate the total size needed for the data vector, convert to chat
    size_t totalSize = static_cast<size_t>(video.noFrames) * video.channels * video.width * video.height;
    video.data.resize(totalSize);

    std::cout << "Size of data vector: " << video.data.size() << std::endl;
        // Read the pixel data into the data vector
    binVideo.read(reinterpret_cast<char*>(video.data.data()), totalSize);
    if (!binVideo) {
        cerr << "Error: Failed to read pixel data." << endl;
        return false;
    }
    binVideo.close();

    // print all the pixel data
    // for (size_t i = 0; i < 16384; ++i) {
    //     std::cout << static_cast<int>(video.data[i]) << " ";
    // }
    // std::cout << std::endl;

    return true;
}


bool readbin2(const std::string& filename, Video2& video)
{
    std::ifstream binVideo(filename, std::ios::binary);
    if (!binVideo.is_open()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return false; 
    }

    // Read header
    binVideo.read(reinterpret_cast<char*>(&video.noFrames), sizeof(video.noFrames));
    binVideo.read(reinterpret_cast<char*>(&video.channels), sizeof(video.channels));
    binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
    binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));

    std::cout << "Number of frames: " << video.noFrames << std::endl;
    std::cout << "Number of channels: " 
              << static_cast<int>(video.channels) << std::endl;
    std::cout << "Width: " << static_cast<int>(video.width) << std::endl;
    std::cout << "Height: " << static_cast<int>(video.height) << std::endl;

    // Each frame has (channels * width * height) pixels
    size_t frameSize = static_cast<size_t>(video.channels) 
                     * static_cast<size_t>(video.width) 
                     * static_cast<size_t>(video.height);

    // Resize outer vector to hold all frames
    video.data.resize(video.noFrames);

    // Read each frame separately
    for (long f = 0; f < video.noFrames; ++f) {
        video.data[f].resize(frameSize); 
        binVideo.read(reinterpret_cast<char*>(video.data[f].data()), frameSize);

        if (!binVideo) {
            std::cerr << "Error: Failed to read pixel data for frame " << f << std::endl;
            return false;
        }
    }

    // Print the data (all frames, all pixels)
    // This might be very large for real videos, so be cautious:
    // std::cout << "Printing all pixel data (frame by frame):\n";
    // for (long f = 0; f < video.noFrames; ++f) {
    //     std::cout << "Frame " << f << ": ";
    //     for (size_t i = 0; i < video.data[f].size(); ++i) {
    //         std::cout << static_cast<int>(video.data[f][i]) << " ";
    //     }
    //     std::cout << std::endl;
    // }

    binVideo.close();
    return true;
}


bool readbin3(const std::string& filename, Video3& video)
{
    std::ifstream binVideo(filename, std::ios::binary);
    if (!binVideo.is_open()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return false;
    }

    // Read header
    binVideo.read(reinterpret_cast<char*>(&video.noFrames), sizeof(video.noFrames));
    binVideo.read(reinterpret_cast<char*>(&video.channels), sizeof(video.channels));
    binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
    binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));

    std::cout << "Number of frames: " << video.noFrames << std::endl;
    std::cout << "Number of channels: " 
              << static_cast<int>(video.channels) << std::endl;
    std::cout << "Width: " << static_cast<int>(video.width) << std::endl;
    std::cout << "Height: " << static_cast<int>(video.height) << std::endl;

    // Each channel has (width * height) pixels
    size_t channelSize = static_cast<size_t>(video.width) 
                       * static_cast<size_t>(video.height);

    // Resize outer vector for the frames
    video.data.resize(video.noFrames);

    // For each frame, allocate channels
    for (long f = 0; f < video.noFrames; ++f) {
        video.data[f].resize(video.channels);

        // For each channel in the frame, allocate memory and read
        for (int c = 0; c < video.channels; ++c) {
            video.data[f][c].resize(channelSize);
            binVideo.read(reinterpret_cast<char*>(video.data[f][c].data()), channelSize);

            if (!binVideo) {
                std::cerr << "Error: Failed to read pixel data for frame " 
                          << f << ", channel " << c << std::endl;
                return false;
            }
        }
    }
    //just print every byte of written file
    
    std::cout << std::endl;
   

    //print the dimesnsion of the data vector and size of subvectors
    std::cout << "Size of data vector: " << video.data.size() << std::endl;
    std::cout << "Size of subvectors: " << video.data[0][0].size() << std::endl;


    // Print the data
    // Again, can be quite large if noFrames is big
    // std::cout << "Printing all pixel data ([frame][channel][index]):\n";
    // for (long f = 0; f < video.noFrames; ++f) {
    //     std::cout << "Frame " << f << ":\n";
    //     for (int c = 0; c < video.channels; ++c) {
    //         std::cout << "  Channel " << c << ": ";
    //         for (size_t i = 0; i < video.data[f][c].size(); ++i) {
    //             std::cout << static_cast<int>(video.data[f][c][i]) << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }

    binVideo.close();
    return true;
}




bool writebin1(const std::string& filename, const Video1& video) {
    // Create an ofstream object to open the file in binary mode
    std::ofstream binVideo(filename, std::ios::binary);
    if (!binVideo.is_open()) {
        std::cerr << "Error: Failed to create or open file " << filename << std::endl;
        return false; // Return false if the file cannot be opened
    }

    // Write metadata to the file
    binVideo.write(reinterpret_cast<const char*>(&video.noFrames), sizeof(video.noFrames));
    binVideo.write(reinterpret_cast<const char*>(&video.channels), sizeof(video.channels));
    binVideo.write(reinterpret_cast<const char*>(&video.width), sizeof(video.width));
    binVideo.write(reinterpret_cast<const char*>(&video.height), sizeof(video.height));

    // Write the data vector to the file
    binVideo.write(reinterpret_cast<const char*>(video.data.data()), video.data.size());

    // Close the file
    binVideo.close();

    //print every pixel of the written file
    // for (size_t i = 0; i < video.data.size(); ++i) {
    //     std::cout << static_cast<int>(video.data[i]) << " ";
    // }

    std::cout << "Data successfully written to " << filename << std::endl;
    return true;
}

bool writebin2(const std::string& filename, const Video2& video) {
    // Create an ofstream object to open the file in binary mode
    std::ofstream binVideo(filename, std::ios::binary);
    if (!binVideo.is_open()) {
        std::cerr << "Error: Failed to create or open file " << filename << std::endl;
        return false; // Return false if the file cannot be opened
    }

    // Write metadata to the file
    binVideo.write(reinterpret_cast<const char*>(&video.noFrames), sizeof(video.noFrames));
    binVideo.write(reinterpret_cast<const char*>(&video.channels), sizeof(video.channels));
    binVideo.write(reinterpret_cast<const char*>(&video.width), sizeof(video.width));
    binVideo.write(reinterpret_cast<const char*>(&video.height), sizeof(video.height));

    // Write each frame separately
    for (long f = 0; f < video.noFrames; ++f) {
        binVideo.write(reinterpret_cast<const char*>(video.data[f].data()), video.data[f].size());
    }

    // Close the file
    binVideo.close();

    std::cout << "Data successfully written to " << filename << std::endl;
    return true;
}

bool writebin3(const std::string& filename, const Video3& video) {
    // Create an ofstream object to open the file in binary mode
    std::ofstream binVideo(filename, std::ios::binary);
    if (!binVideo.is_open()) {
        std::cerr << "Error: Failed to create or open file " << filename << std::endl;
        return false; // Return false if the file cannot be opened
    }

    // Write metadata to the file
    binVideo.write(reinterpret_cast<const char*>(&video.noFrames), sizeof(video.noFrames));
    binVideo.write(reinterpret_cast<const char*>(&video.channels), sizeof(video.channels));
    binVideo.write(reinterpret_cast<const char*>(&video.width), sizeof(video.width));
    binVideo.write(reinterpret_cast<const char*>(&video.height), sizeof(video.height));

    // Write each frame separately
    for (long f = 0; f < video.noFrames; ++f) {
        for (int c = 0; c < video.channels; ++c) {
            binVideo.write(reinterpret_cast<const char*>(video.data[f][c].data()), video.data[f][c].size());
        }
    }

    // Close the file
    binVideo.close();

    std::cout << "Data successfully written to " << filename << std::endl;
    return true;
}


bool reverseVid(const std::string& input, const std::string& output, const std::string& mode)
{
    // if (mode == "-S") {
    //     // do path from top to bottom and copy to new vector
    //     Video1 video;
    //     if (!readbin1(input, video)) {
    //         std::cerr << "Failed to read the input video file." << std::endl;
    //         return false;
    //     }

    //     // calculate frame size
    //     size_t frameSize = static_cast<size_t>(video.channels) 
    //                      * static_cast<size_t>(video.width) 
    //                      * static_cast<size_t>(video.height);

    //     if (video.data.size() % frameSize != 0) {
    //         std::cerr << "Error: Invalid frame size. Not divisible" << std::endl;
    //         return false;
    //     }

    //     int numFrames = video.noFrames;
    //     std::vector<int> reversedVid(video.data.size());

    //     for (int i = 0; i < numFrames; ++i) {
    //     int startIdxOriginal = i * frameSize;
    //     int startIdxReversed = (numFrames - 1 - i) * frameSize;


    //     std::copy(video.data.begin() + startIdxOriginal,
    //               video.data.begin() + startIdxOriginal + frameSize,
    //               reversedVid.begin() + startIdxReversed);


    //     // now write the reversed video to the output file
    //     }
    //     cout << "Video reversed successfully! S" << endl;

    // } 
    if (mode == "-S") {
        // use cpp reverse on 2d
        Video2 video;
        if (!readbin2(input, video)) {
            std::cerr << "Failed to read the input video file." << std::endl;
            return false;
        }

        reverse(video.data.begin(), video.data.end());
        writebin2(output, video);

    }
    
    else if (mode == "-M") {
        // swap inner and outer frames in 1d array
        Video1 video;
        if (!readbin1(input, video)) {
            std::cerr << "Failed to read the input video file." << std::endl;
            return false;
        }
        size_t frameSize = static_cast<size_t>(video.channels) 
                     * static_cast<size_t>(video.width) 
                     * static_cast<size_t>(video.height);

        long start = 0;
        long end   = video.noFrames - 1;

        while (start < end) {
        // Offsets for each of the two frames we’re swapping
        size_t startOffset = static_cast<size_t>(start) * frameSize;
        size_t endOffset   = static_cast<size_t>(end)   * frameSize;

        // Swap the entire block of frameSize bytes
        for (size_t i = 0; i < frameSize; ++i) {
            std::swap(video.data[startOffset + i], 
                      video.data[endOffset   + i]);
        }

        // Move inward
        ++start;
        --end;

    }
    cout << "Video reversed successfully! M" << endl;
    writebin1(output, video);
    

    } else {
        std::cerr << "Error: Invalid mode. Use -S or -M" << std::endl;
        return false;
    }

    return true;
}