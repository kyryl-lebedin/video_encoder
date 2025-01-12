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
  vector<unsigned char> data1;
  vector<vector<unsigned char>> data2;
  vector<vector<vector<unsigned char>>> data3;
  enum StorageMode { Mode1D, Mode2D, Mode3D } mode;
};

bool readbin(const string& filename, Video& video);
// bool readbinS(const string& filename, Video1& video);
// bool readbinH(const string& filename, Video1& video);

bool writebin(const string& filename, const Video& video);
// bool writebinS(const string& filename, const Video& video);
// bool writebinH(const string& filename, const Video& video);

// indeces 1-3 determine the structure of created vector to hold pixels 1d, 2d
// or 3d
struct Video1 {
  long noFrames;
  unsigned char channels;
  unsigned char height;
  unsigned char width;
  vector<unsigned char> data;
};

struct Video2 {
  long noFrames;
  unsigned char channels;
  unsigned char height;
  unsigned char width;
  vector<vector<unsigned char>> data;
};

struct Video3 {
  long noFrames;
  unsigned char channels;
  unsigned char height;
  unsigned char width;
  vector<vector<vector<unsigned char>>> data;
};

bool readbin1(const string& filename, Video1& video);

bool readbin2(const string& filename, Video2& video);

bool readbin2S(const string& filename, Video2& video);

bool readbin3(const string& filename, Video3& video);

bool writebin1(const string& filename, const Video1& video);

bool writebin2(const string& filename, const Video2& video);

bool writebin2S(const string& filename, const Video2& video);

bool writebin3(const string& filename, const Video3& video);

bool reverse(const string& input, const string& output, const string& mode);

bool swap_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel1, unsigned char channel2);

bool clip_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel, unsigned char min, unsigned char max);

bool scale_channel(const string& input, const string& output,
                   const string& mode, unsigned char channel, float factor);

#endif
