#include <chrono>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "FilmMaster2000.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  // add check for argv[0]?

  string input = argv[1];
  string output = argv[2];

  if (!input.ends_with(".bin") || !output.ends_with(".bin")) {
    std::cerr << "Error: Invalid input or output. Both input and output files "
                 "must be binary."
              << std::endl;
    return 1;
  }

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
  //   cout << opt << " ";
  // }
  // cout << endl;
  // cout << options[0] << endl;

  if (function == "reverse") {
    // auto start = chrono::high_resolution_clock::now();
    if (options.size() != 0) {
      cerr << "Invalid number of arguments for reverse function. Expected 0 "
              "arguments."
           << endl;
      return 1;
    }

    if (!reverse(input, output, mode)) {
      return 1;
    }
    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> elapsed = end - start;
    // cout << "Time taken for reverse function: " << elapsed.count() <<
    // "seconds "
    //  << endl;
  } else if (function == "swap_channel") {
    if (options.size() != 1 ||
        !std::regex_match(options[0], std::regex(R"(\d+,\d+)"))) {
      std::cerr << "Invalid syntax. Expected: './runme [input] [output] [mode] "
                   "swap_channel [channel1],[channel2]'."
                << std::endl;
      return 1;
    }

    unsigned char channel1, channel2;

    stringstream ss(options[0]);
    int tempChannel1, tempChannel2;

    char delimiter;

    ss >> tempChannel1 >> delimiter >> tempChannel2;

    channel1 = static_cast<unsigned char>(tempChannel1);
    channel2 = static_cast<unsigned char>(tempChannel2);

    auto start = chrono::high_resolution_clock::now();

    // the check of if the channel is in the range is done in the
    // swap_channel function
    if (!swap_channel(input, output, mode, channel1, channel2)) {
      return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time taken for swap_channels: " << elapsed.count() << " seconds"
         << endl;
  } else if (function == "clip_channel") {
    if (options.size() != 2 ||
        !std::regex_match(options[0], std::regex(R"(\d+)")) ||
        !std::regex_match(options[1], std::regex(R"(\[\d+,\d+\])"))) {
      std::cerr << "Invalid syntax. Expected: './runme [input] [output] [mode] "
                   "clip_channel [channel] [[min],[max]]'."
                << std::endl;
      return 1;
    }

    unsigned char max, min;
    options[1].erase(0, 1);
    options[1].erase(options[1].size() - 1, 1);

    istringstream stream(options[1]);
    int tempMin, tempMax;
    char comma;

    stream >> tempMin >> comma >> tempMax;

    if (tempMin > tempMax || tempMin < 0 || tempMax > 255) {
      cerr << " Invalid range, allowed: 0-255; min < max." << endl;
      return 1;
    }

    min = static_cast<unsigned char>(tempMin);
    max = static_cast<unsigned char>(tempMax);

    auto start = chrono::high_resolution_clock::now();

    // channel validity is checked in the clip_channel function
    if (!clip_channel(input, output, mode,
                      static_cast<unsigned char>(stoi(options[0])), min, max)) {
      return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time taken for clip_channel: " << elapsed.count() << " seconds"
         << endl;

  }

  else if (function == "scale_channel") {
    if (options.size() != 2 ||
        !std::regex_match(options[0], std::regex(R"(\d+)")) ||
        !std::regex_match(options[1], std::regex(R"(-?\d+(\.\d+)?)"))) {
      std::cerr << "Invalid syntax. Expected: './runme [input] [output] [mode] "
                   "scale_channel [channel] [factor]'."
                << std::endl;
      return 1;
    }

    unsigned char channel = static_cast<unsigned char>(stoi(options[0]));
    float scale_factor = stof(options[1]);

    auto start = chrono::high_resolution_clock::now();

    // channel validity is checked in the scale_channel function
    if (!scale_channel(input, output, mode, channel, scale_factor)) {
      return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time taken for scale_channel: " << elapsed.count() << " seconds"
         << endl;

  }

  else {
    cerr << "Unknown function. Use available functions: reverse, "
            "swap_channel, "
            "clip_channel, scale_channel.\n"
         << endl;
    return 1;
  }

  return 0;
}
