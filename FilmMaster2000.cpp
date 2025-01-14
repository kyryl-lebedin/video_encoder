#include "FilmMaster2000.hpp"

#include <immintrin.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

class ThrottledStream {
  std::ifstream& stream;
  int delayMs;

 public:
  ThrottledStream(std::ifstream& stream, int delayMs)
      : stream(stream), delayMs(delayMs) {}

  void read(char* buffer, std::streamsize size) {
    stream.read(buffer, size);
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
  }

  bool good() const { return stream.good(); }
};

bool readbin(const string& filename, Video& video, Video::StorageMode mode) {
  video.mode = mode;

  ifstream binVideo(filename, ios::binary);
  if (!binVideo.is_open()) {
    cerr << "Error: Failed to open file " << filename << endl;
    return false;
  }

  binVideo.read(reinterpret_cast<char*>(&video.noFrames),
                sizeof(video.noFrames));
  binVideo.read(reinterpret_cast<char*>(&video.channels),
                sizeof(video.channels));
  binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
  binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));

  if (mode == Video::Mode1D) {
    size_t totalSize = static_cast<size_t>(video.noFrames) * video.channels *
                       video.width * video.height;
    video.data1.resize(totalSize);
    binVideo.read(reinterpret_cast<char*>(video.data1.data()), totalSize);
  }
  if (mode == Video::Mode2D) {
    size_t frameSize = static_cast<size_t>(video.channels) *
                       static_cast<size_t>(video.width) *
                       static_cast<size_t>(video.height);
    video.data2.resize(video.noFrames);
    for (long f = 0; f < video.noFrames; ++f) {
      video.data2[f].resize(frameSize);
      binVideo.read(reinterpret_cast<char*>(video.data2[f].data()), frameSize);
    }
  }

  if (mode == Video::Mode3D) {
    size_t channelSize =
        static_cast<size_t>(video.width) * static_cast<size_t>(video.height);

    video.data3.resize(video.noFrames);
    for (long f = 0; f < video.noFrames; ++f) {
      video.data3[f].resize(video.channels);
      for (int c = 0; c < video.channels; ++c) {
        video.data3[f][c].resize(channelSize);
        binVideo.read(reinterpret_cast<char*>(video.data3[f][c].data()),
                      channelSize);
      }
    }
  }

  binVideo.close();
  return true;
}

bool readbinS(const string& filename, Video& video, Video::StorageMode mode) {
  video.mode = mode;

  ifstream binVideo(filename, ios::binary);
  if (!binVideo.is_open()) {
    cerr << "Error: Failed to open file " << filename << endl;
    return false;
  }

  size_t headerSize = sizeof(video.noFrames) + 3 * sizeof(unsigned char);

  vector<char> headerData(headerSize);
  binVideo.read(headerData.data(), headerSize);

  char* ptr = headerData.data();
  memcpy(&video.noFrames, ptr, sizeof(video.noFrames));
  ptr += sizeof(video.noFrames);
  memcpy(&video.channels, ptr, sizeof(video.channels));
  ptr += sizeof(video.channels);
  memcpy(&video.width, ptr, sizeof(video.width));
  ptr += sizeof(video.width);
  memcpy(&video.height, ptr, sizeof(video.height));

  size_t frameSize = static_cast<size_t>(video.channels) *
                     static_cast<size_t>(video.width) *
                     static_cast<size_t>(video.height);
  size_t totalSize = static_cast<size_t>(video.noFrames) * frameSize;
  size_t channelSize =
      static_cast<size_t>(video.width) * static_cast<size_t>(video.height);

  vector<unsigned char> alldata(totalSize);
  binVideo.read(reinterpret_cast<char*>(alldata.data()), totalSize);
  binVideo.close();

  if (mode == Video::Mode1D) {
    video.data1.resize(totalSize);
    std::copy(alldata.begin(), alldata.end(), video.data1.begin());
  }

  if (mode == Video::Mode2D) {
    video.data2.resize(video.noFrames);
    for (long f = 0; f < video.noFrames; ++f) {
      video.data2[f] =
          vector<unsigned char>(alldata.begin() + f * frameSize,
                                alldata.begin() + (f + 1) * frameSize);
    }
  }

  if (mode == Video::Mode3D) {
    video.data3.resize(video.noFrames);
    for (long f = 0; f < video.noFrames; ++f) {
      video.data3[f].resize(video.channels);
      for (int c = 0; c < video.channels; ++c) {
        video.data3[f][c].resize(channelSize);
        memcpy(video.data3[f][c].data(),
               alldata.data() + f * frameSize + c * channelSize, channelSize);
      }
    }
  }

  return true;
}

bool writebin(const string& filename, Video& video, Video::StorageMode mode) {
  video.mode = mode;

  ofstream binVideo(filename, ios::binary);
  if (!binVideo.is_open()) {
    cerr << "Error: Failed to create or open file " << filename << endl;
    return false;
  }

  binVideo.write(reinterpret_cast<const char*>(&video.noFrames),
                 sizeof(video.noFrames));
  binVideo.write(reinterpret_cast<const char*>(&video.channels),
                 sizeof(video.channels));
  binVideo.write(reinterpret_cast<const char*>(&video.width),
                 sizeof(video.width));
  binVideo.write(reinterpret_cast<const char*>(&video.height),
                 sizeof(video.height));

  if (mode == Video::Mode1D) {
    binVideo.write(reinterpret_cast<const char*>(video.data1.data()),
                   video.data1.size());
  }
  if (mode == Video::Mode2D) {
    for (long f = 0; f < video.noFrames; ++f) {
      binVideo.write(reinterpret_cast<const char*>(video.data2[f].data()),
                     video.data2[f].size());
    }
  }
  if (mode == Video::Mode3D) {
    for (long f = 0; f < video.noFrames; ++f) {
      for (int c = 0; c < video.channels; ++c) {
        binVideo.write(reinterpret_cast<const char*>(video.data3[f][c].data()),
                       video.data3[f][c].size());
      }
    }
  }

  binVideo.close();
  return true;
}

bool writebinS(const string& filename, Video& video, Video::StorageMode mode) {
  video.mode = mode;

  ofstream binVideo(filename, ios::binary);
  if (!binVideo.is_open()) {
    cerr << "Error: Failed to create or open file " << filename << endl;
    return false;
  }

  // binVideo.write(reinterpret_cast<const char*>(&video.noFrames),
  //                sizeof(video.noFrames));
  // binVideo.write(reinterpret_cast<const char*>(&video.channels),
  //                sizeof(video.channels));
  // binVideo.write(reinterpret_cast<const char*>(&video.width),
  //                sizeof(video.width));
  // binVideo.write(reinterpret_cast<const char*>(&video.height),
  //                sizeof(video.height));

  size_t headerSize = sizeof(video.noFrames) + 3 * sizeof(unsigned char);
  vector<char> headerData(headerSize);

  char* ptr = headerData.data();
  memcpy(ptr, &video.noFrames, sizeof(video.noFrames));
  ptr += sizeof(video.noFrames);

  memcpy(ptr, &video.channels, sizeof(video.channels));
  ptr += sizeof(video.channels);

  memcpy(ptr, &video.width, sizeof(video.width));
  ptr += sizeof(video.width);

  memcpy(ptr, &video.height, sizeof(video.height));

  binVideo.write(headerData.data(), headerSize);

  if (mode == Video::Mode1D) {
    binVideo.write(reinterpret_cast<const char*>(video.data1.data()),
                   video.data1.size());
  }
  if (mode == Video::Mode2D) {
    size_t frameSize = static_cast<size_t>(video.channels) *
                       static_cast<size_t>(video.width) *
                       static_cast<size_t>(video.height);

    size_t totalSize = static_cast<size_t>(video.noFrames) * frameSize;
    vector<unsigned char> alldata(totalSize);
    for (long f = 0; f < video.noFrames; ++f) {
      memcpy(&alldata[f * frameSize], video.data2[f].data(), frameSize);
    }

    binVideo.write(reinterpret_cast<const char*>(alldata.data()), totalSize);
  }

  if (mode == Video::Mode3D) {
    for (long f = 0; f < video.noFrames; ++f) {
      for (int c = 0; c < video.channels; ++c) {
        binVideo.write(reinterpret_cast<const char*>(video.data3[f][c].data()),
                       video.data3[f][c].size());
      }
    }
  }

  binVideo.close();
  return true;
}

bool reverse(const string& input, const string& output, const string& mode) {
  if (mode == "-M") {
    ifstream video(input, ios::binary);
    if (!video.is_open()) {
      cerr << "Error: Failed to open file " << input << endl;
      return false;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    ofstream reversedVideo(output, ios::binary);
    if (!reversedVideo.is_open()) {
      cerr << "Error: Failed to open file " << output << endl;
      return false;
    }

    reversedVideo.write(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    reversedVideo.write(reinterpret_cast<char*>(&channels), sizeof(channels));
    reversedVideo.write(reinterpret_cast<char*>(&height), sizeof(height));
    reversedVideo.write(reinterpret_cast<char*>(&width), sizeof(width));

    size_t frameSize = static_cast<size_t>(channels) *
                       static_cast<size_t>(width) * static_cast<size_t>(height);

    // maybe just unisgned char
    vector<char> frameBuffer(frameSize);

    streamoff headerSize = sizeof(noFrames) + 3 * sizeof(unsigned char);

    for (long i = noFrames - 1; i >= 0; --i) {
      streamoff offset =
          headerSize + static_cast<std::streamoff>(i) * frameSize;
      video.seekg(offset, ios::beg);
      video.read(frameBuffer.data(), frameSize);
      reversedVideo.write(frameBuffer.data(), frameSize);
    }
    video.close();
    reversedVideo.close();
  } else {
    Video video;
    if (mode == "-S") {
      if (!readbinS(input, video, Video::Mode2D)) {
        return false;
      }
    } else {
      if (!readbin(input, video, Video::Mode2D)) {
        return false;
      }
    }

    std::reverse(video.data2.begin(), video.data2.end());

    if (mode == "-S") {
      if (!writebinS(output, video, Video::Mode2D)) {
        return false;
      }
    } else {
      if (!writebin(output, video, Video::Mode2D)) {
        return false;
      }
    }
  }
  return true;
}

bool swap_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel1, unsigned char channel2) {
  if (mode == "-M") {
    ifstream video(input, ios::binary);
    if (!video.is_open()) {
      cerr << "Error: Failed to open file " << input << endl;
      return false;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    if (channel1 > channels || channel2 > channels) {
      cerr << "Error: Invalid channel number." << endl;
      video.close();
      return false;
    }

    ofstream swappedVideo(output, ios::binary);
    if (!swappedVideo.is_open()) {
      cerr << "Error: Failed to open file " << output << endl;
      return false;
    }

    swappedVideo.write(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    swappedVideo.write(reinterpret_cast<char*>(&channels), sizeof(channels));
    swappedVideo.write(reinterpret_cast<char*>(&height), sizeof(height));
    swappedVideo.write(reinterpret_cast<char*>(&width), sizeof(width));

    size_t channelSize =
        static_cast<size_t>(width) * static_cast<size_t>(height);

    vector<vector<char>> frameBuffer(channels, vector<char>(channelSize));

    // swap two channels in frame and write to output
    for (long i = 0; i < noFrames; ++i) {
      for (int j = 0; j < channels; ++j) {
        video.read(frameBuffer[j].data(), channelSize);
      }

      swap(frameBuffer[channel1 - 1], frameBuffer[channel2 - 1]);

      for (int j = 0; j < channels; ++j) {
        swappedVideo.write(frameBuffer[j].data(), channelSize);
      }
    }

    video.close();
    swappedVideo.close();

  } else {
    Video video;

    if (mode == "-S") {
      if (!readbinS(input, video, Video::Mode3D)) {
        return false;
      }
    } else {
      if (!readbin(input, video, Video::Mode3D)) {
        return false;
      }
    }

    if (channel1 > video.channels || channel2 > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      std::swap(video.data3[f][channel1 - 1], video.data3[f][channel2 - 1]);
    }

    if (mode == "-S") {
      if (!writebinS(output, video, Video::Mode3D)) {
        return false;
      }
    } else {
      if (!writebin(output, video, Video::Mode3D)) {
        return false;
      }
    }
  }

  return true;
}

bool clip_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel, unsigned char min, unsigned char max) {
  if (mode == "-S") {
    Video video;
    if (!readbinS(input, video, Video::Mode3D)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data3[f][channel - 1][i] < min) {
          video.data3[f][channel - 1][i] = min;
        } else if (video.data3[f][channel - 1][i] > max) {
          video.data3[f][channel - 1][i] = max;
        }
      }
    }

    if (!writebinS(output, video, Video::Mode3D)) {
      return false;
    }

  } else if (mode == "-M") {
    ifstream video(input, ios::binary);
    if (!video.is_open()) {
      cerr << "Error: Failed to open file " << input << endl;
      return false;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    if (channel > channels) {
      cerr << "Error: Invalid channel number." << endl;
      video.close();
      return false;
    }

    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    ofstream clippedVideo(output, ios::binary);
    if (!clippedVideo.is_open()) {
      cerr << "Error: Failed to open file " << output << endl;
      return false;
    }

    clippedVideo.write(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    clippedVideo.write(reinterpret_cast<char*>(&channels), sizeof(channels));
    clippedVideo.write(reinterpret_cast<char*>(&height), sizeof(height));
    clippedVideo.write(reinterpret_cast<char*>(&width), sizeof(width));

    size_t frameSize = static_cast<size_t>(channels) *
                       static_cast<size_t>(width) * static_cast<size_t>(height);

    size_t channelSize =
        static_cast<size_t>(width) * static_cast<size_t>(height);

    std::vector<unsigned char> frameBuffer(frameSize);
    size_t pixelOffset = channelSize * (channel - 1);
    // iterate through every pixel of channel
    for (long i = 0; i < noFrames; ++i) {
      video.read(reinterpret_cast<char*>(frameBuffer.data()), frameSize);
      for (size_t j = pixelOffset; j < pixelOffset + channelSize; ++j) {
        if (frameBuffer[j] < min) {
          frameBuffer[j] = min;
        } else if (frameBuffer[j] > max) {
          frameBuffer[j] = max;
        }
      }

      clippedVideo.write(reinterpret_cast<char*>(frameBuffer.data()),
                         frameSize);
    }

    video.close();
    clippedVideo.close();

  } else if (mode == "-V") {
    Video video;
    if (!readbin(input, video, Video::Mode3D)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data3[f][channel - 1][i] < min) {
          video.data3[f][channel - 1][i] = min;
        } else if (video.data3[f][channel - 1][i] > max) {
          video.data3[f][channel - 1][i] = max;
        }
      }
    }
    if (!writebin(output, video, Video::Mode3D)) {
      return false;
    }
  }
  return true;
}

bool scale_channel(const string& input, const string& output,
                   const string& mode, unsigned char channel, float factor) {
  if (mode == "-M") {
    ifstream video(input, ios::binary);
    if (!video.is_open()) {
      cerr << "Error: Failed to open file " << input << endl;
      return false;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    if (channel > channels) {
      cerr << "Error: Invalid channel number." << endl;
      video.close();
      return false;
    }

    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    ofstream clippedVideo(output, ios::binary);
    if (!clippedVideo.is_open()) {
      cerr << "Error: Failed to open file " << output << endl;
      return false;
    }

    clippedVideo.write(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    clippedVideo.write(reinterpret_cast<char*>(&channels), sizeof(channels));
    clippedVideo.write(reinterpret_cast<char*>(&height), sizeof(height));
    clippedVideo.write(reinterpret_cast<char*>(&width), sizeof(width));

    size_t frameSize = static_cast<size_t>(channels) *
                       static_cast<size_t>(width) * static_cast<size_t>(height);

    size_t channelSize =
        static_cast<size_t>(width) * static_cast<size_t>(height);

    std::vector<unsigned char> frameBuffer(frameSize);
    size_t pixelOffset = channelSize * (channel - 1);
    // iterate through every pixel of channel
    for (long f = 0; f < noFrames; ++f) {
      video.read(reinterpret_cast<char*>(frameBuffer.data()), frameSize);
      for (size_t j = pixelOffset; j < pixelOffset + channelSize; ++j) {
        // read frame, access pixel in flatenned frame, starting at j
        if (frameBuffer[j] != 0) {
          if (factor * frameBuffer[j] > 255) {
            frameBuffer[j] = 255;
          } else if (factor <= 0) {
            frameBuffer[j] = 0;
          } else {
            frameBuffer[j] = frameBuffer[j] * factor;
          }
        }
      }
      clippedVideo.write(reinterpret_cast<char*>(frameBuffer.data()),
                         frameSize);
    }

    video.close();
    clippedVideo.close();
  } else {
    Video video;

    if (mode == "-S") {
      if (!readbinS(input, video, Video::Mode3D)) {
        return false;
      }
    } else {
      if (!readbin(input, video, Video::Mode3D)) {
        return false;
      }
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data3[f][channel - 1][i] != 0) {
          if (factor * video.data3[f][channel - 1][i] > 255) {
            video.data3[f][channel - 1][i] = 255;
          } else if (factor <= 0) {
            video.data3[f][channel - 1][i] = 0;
          }

          else {
            video.data3[f][channel - 1][i] =
                video.data3[f][channel - 1][i] * factor;
          }
        }
      }
    }

    if (mode == "-S") {
      if (!writebinS(output, video, Video::Mode3D)) {
        return false;
      }
    } else {
      if (!writebin(output, video, Video::Mode3D)) {
        return false;
      }
    }
  }
  return true;
}
