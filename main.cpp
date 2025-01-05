#include <iostream>
#include <string>
#include <vector>

#include "FilmMaster2000.hpp"
// #include <chrono>
#include <sstream>

using namespace std;

int main(int argc, char *argv[]) {
  string input = argv[1];
  string output = argv[2];
  string mode = "-V";
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

  // cout << "Input file: " << input << endl;
  // cout << "Output file: " << output << endl;
  // cout << "Mode: " << (mode.empty() ? "-V" : mode) << endl;
  // cout << "Function: " << function << endl;
  // cout << "Options: ";
  // for (const string &opt : options) {
  //     cout << opt << " ";
  // }
  // cout << endl;

  if (function == "reverse") {
    // add time measurement
    // auto start = chrono::high_resolution_clock::now();
    reverse(input, output, mode);
    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> elapsed = end - start;
    // cout << "Time taken for reverse function: " << elapsed.count() << "
    // seconds" << endl;
  } else if (function == "swap_channel") {
    unsigned char channel1 = static_cast<unsigned char>(stoi(options[0]));
    unsigned char channel2 = static_cast<unsigned char>(stoi(options[1]));
    // auto start = chrono::high_resolution_clock::now();
    swap_channel(input, output, mode, channel1, channel2);
    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> elapsed = end - start;
    // cout << "Time taken for swap_channels: " << elapsed.count() << " seconds"
    // << endl;
  } else if (function == "clip_channel") {
    unsigned char max, min;
    options[1].erase(0, 1);
    options[1].erase(options[1].size() - 1, 1);

    istringstream stream(options[1]);
    int tempMin, tempMax;
    char comma;

    stream >> tempMin >> comma >> tempMax;

    // if (stream.fail() || comma != ',' || tempMin < 0 || tempMax < 0 ||
    // tempMin > 128 || tempMax > 128) {
    //     throw std::runtime_error("Invalid input. The range to clip pixels is
    //     0-128.");
    // }

    min = static_cast<unsigned char>(tempMin);
    max = static_cast<unsigned char>(tempMax);

    cout << "Min: " << static_cast<int>(min) << endl;

    // auto start = chrono::high_resolution_clock::now();
    clip_channel(input, output, mode,
                 static_cast<unsigned char>(stoi(options[0])), min, max);
    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> elapsed = end - start;
    // cout << "Time taken for clip_channel: " << elapsed.count() << " seconds"
    // << endl;

  }

  else if (function == "scale_channel") {
    unsigned char channel = static_cast<unsigned char>(stoi(options[0]));
    float scale_factor = stof(options[1]);

    // auto start = chrono::high_resolution_clock::now();
    scale_channel(input, output, mode, channel, scale_factor);
    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> elapsed = end - start;
    // cout << "Time taken for scale_channel: " << elapsed.count() << " seconds"
    // << endl;

  }

  else {
    cerr << "Unknown function. Use available functions: reverse, swap_channel, "
            "clip_channel, scale_channel.\n"
         << endl;
    return 1;
  }

  return 0;
}
