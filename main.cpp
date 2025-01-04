#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "FilmMaster2000.hpp" 

using namespace std;

int main(int argc, char *argv[]){
    

    string input = argv[1];
    string output = argv[2];
    string mode = argv[3];
    string function = argv[4];
    // access funciton arguments directly thorugh argv

   

    // Input type correctness
    if(mode != "-S" && mode != "-M"){
        cerr << "Unknown function mode. Usage: ./runme [input] [output] [-S/-M] [function] [arguments(optional)]\n" << endl;
        return 1;
    }
    if (function != "reverse" && function != "swap_channel" 
    && function != "clip_channel" && function != "scale_channel"){
        cerr << "Unknown function. Use available functions: reverse, swap_channel, clip_channel, scale_channel.\n" << endl;
        return 1;
    }
   
    if (function == "reverse") {
        reverseVid(input, output, mode);
    }

    
    if (function == "swap_channel") {
        unsigned char channel1 = static_cast<unsigned char>(stoi(argv[5]));
        unsigned char channel2 = static_cast<unsigned char>(stoi(argv[6]));
        swapChannel(input, output, mode, channel1, channel2);
    }
    
    return 0;
}
