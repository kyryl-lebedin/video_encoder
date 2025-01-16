// Copyright 2025 Kyryl Lebedin
#ifndef FILMMASTER2000_HPP
#define FILMMASTER2000_HPP

#include <string>
#include <vector>

using namespace std;

struct Video {
  long noFrames;
  unsigned char channels;
  unsigned char height;
  unsigned char width;
  // no memory is allocated to vectors in this initialization
  // the data struture for pixels will be chosen upon function call
  // memory will be allocated once vectors are resized
  vector<unsigned char> data1;
  vector<vector<unsigned char>> data2;
  vector<vector<vector<unsigned char>>> data3;
  enum StorageMode { Mode1D, Mode2D, Mode3D } mode;
};

bool reverse(const string& input, const string& output, const string& mode);

bool swap_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel1, unsigned char channel2);

bool clip_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel, unsigned char min, unsigned char max);

bool scale_channel(const string& input, const string& output,
                   const string& mode, unsigned char channel, float factor);

#endif
