#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "FilmMaster2000.hpp" 

using namespace std;

int main(int argc, char *argv[]){
    

    string input = argv[1];
    string output = argv[2];
    string mode = "";
    string function;
    vector<string> options;

    int function_name_index = 3; 
    if (argc > 3 && (string(argv[3]) == "-S" || string(argv[3]) == "-M")) {
        mode = argv[3];
        function_name_index = 4; 
    }
   
    function = argv[function_name_index];
    function_name_index++;

    for (int i = function_name_index; i < argc; i++) {
        options.push_back(argv[i]);
    }

    cout << "Input file: " << input << endl;
    cout << "Output file: " << output << endl;
    cout << "Mode: " << (mode.empty() ? "None" : mode) << endl;
    cout << "Function: " << function << endl;
    cout << "Options: ";
    for (const string &opt : options) {
        cout << opt << " ";
    }
    cout << endl;

    
   
    if (function == "reverse") {
        reverse(input, output, mode);
    }
    // else if (function == "swap_channel") {
    //     unsigned char channel1 = static_cast<unsigned char>(stoi(argv[5]));
    //     unsigned char channel2 = static_cast<unsigned char>(stoi(argv[6]));
    //     swap_channel(input, output, mode, channel1, channel2);
    // }
    // else {
    //     cerr << "Unknown function. Use available functions: reverse, swap_channel, clip_channel, scale_channel.\n" << endl;
    //     return 1;
    // }
    
    return 0;
}
