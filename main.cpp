#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "FilmMaster2000.hpp" 

using namespace std;

int main(int argc, char *argv[]){
    // Input correctness check
    if(argc != 5){
        cerr << "Usage: ./runme [input] [output] [-S/-M] [function]\n" << endl;
        return 1;
    }

    string input = argv[1];
    string output = argv[2];
    string mode = argv[3];
    string function = argv[4];

    // Input type correctness
    if(mode != "-S" && mode != "-M"){
        cerr << "Usage: ./runme [input] [output] [-S/-M] [function]\n" << endl;
        return 1;
    }

    if (function != "reverse" && function != "swap_channel" 
    && function != "clip_channel" && function != "scale_channel"){
        cerr << "Unknown function. Use available functions: reverse, swap_channel, clip_channel, scale_channel.\n" << endl;
        return 1;
    }

   // Read input file
   Video video;
   if (!readbin(input, video)) {
        cerr << "Failed to read the input video file." << endl;
        return 1;
    }

    // Successfully read the file
    cout << "Video file read successfully!" << endl;

    
    return 0;
}
