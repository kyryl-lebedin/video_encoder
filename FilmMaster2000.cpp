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

void printVideo1(const Video1& video) {
  cout << "Number of frames: " << video.noFrames << endl;
  cout << "Number of channels: " << static_cast<int>(video.channels) << endl;
  cout << "Width: " << static_cast<int>(video.width) << endl;
  cout << "Height: " << static_cast<int>(video.height) << endl;
  cout << "First frame pixel data: ";
  for (size_t i = 0; i < video.width * video.height * video.channels; ++i) {
    cout << static_cast<int>(video.data[i]) << " ";
  }
  cout << endl;
  cout << "Last frame pixel data: ";
  // for (size_t i = video.data.size() - video.width * video.height *
  // video.channels; i < video.data.size(); ++i) {
  //     cout << static_cast<int>(video.data[i]) << " ";
  // }
}

void printVideo2(const Video2& video) {
  cout << "Number of frames: " << video.noFrames << endl;
  cout << "Number of channels: " << static_cast<int>(video.channels) << endl;
  cout << "Width: " << static_cast<int>(video.width) << endl;
  cout << "Height: " << static_cast<int>(video.height) << endl;
  cout << "First frame pixel data: ";
  for (size_t i = 0; i < video.width * video.height * video.channels; ++i) {
    cout << static_cast<int>(video.data[0][i]) << " ";
  }
  cout << endl;
  cout << "Last frame pixel data: ";
  for (size_t i = video.data[video.noFrames - 1].size() -
                  video.width * video.height * video.channels;
       i < video.data[video.noFrames - 1].size(); ++i) {
    cout << static_cast<int>(video.data[video.noFrames - 1][i]) << " ";
  }
}

void printVideo3(const Video3& video) {
  cout << "Number of frames: " << video.noFrames << endl;
  cout << "Number of channels: " << static_cast<int>(video.channels) << endl;
  cout << "Width: " << static_cast<int>(video.width) << endl;
  cout << "Height: " << static_cast<int>(video.height) << endl;
  cout << "First frame pixel data: ";
  for (size_t i = 0; i < video.width * video.height; ++i) {
    cout << static_cast<int>(video.data[0][0][i]) << " ";
  }
  cout << endl;
  cout << "Last frame pixel data: ";
  for (size_t i = video.data[video.noFrames - 1][video.channels - 1].size() -
                  video.width * video.height;
       i < video.data[video.noFrames - 1][video.channels - 1].size(); ++i) {
    cout << static_cast<int>(
                video.data[video.noFrames - 1][video.channels - 1][i])
         << " ";
  }
}

// indeces 1-3 determine the structure of created vector to hold pixels 1d, 2d
// or 3d

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

  binVideo.read(reinterpret_cast<char*>(&video.noFrames),
                sizeof(video.noFrames));
  binVideo.read(reinterpret_cast<char*>(&video.channels),
                sizeof(video.channels));
  binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
  binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));

  size_t frameSize = static_cast<size_t>(video.channels) *
                     static_cast<size_t>(video.width) *
                     static_cast<size_t>(video.height);
  size_t totalSize = static_cast<size_t>(video.noFrames) * frameSize;

  vector<unsigned char> alldata(totalSize);
  binVideo.read(reinterpret_cast<char*>(alldata.data()), totalSize);
  binVideo.close();

  if (mode == Video::Mode1D) {
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
  }

  return true;
}

bool readbin1(const string& filename, Video1& video) {
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

  size_t totalSize = static_cast<size_t>(video.noFrames) * video.channels *
                     video.width * video.height;
  video.data.resize(totalSize);
  binVideo.read(reinterpret_cast<char*>(video.data.data()), totalSize);

  binVideo.close();

  // printVideo1(video);

  return true;
}

bool readbin2(const string& filename, Video2& video) {
  ifstream binVideo(filename, ios::binary);
  if (!binVideo.is_open()) {
    cerr << "Error: Failed to open file " << filename << endl;
    return false;
  }

  ThrottledStream binVideot(binVideo, 10);

  binVideo.read(reinterpret_cast<char*>(&video.noFrames),
                sizeof(video.noFrames));
  binVideo.read(reinterpret_cast<char*>(&video.channels),
                sizeof(video.channels));
  binVideo.read(reinterpret_cast<char*>(&video.width), sizeof(video.width));
  binVideo.read(reinterpret_cast<char*>(&video.height), sizeof(video.height));

  size_t frameSize = static_cast<size_t>(video.channels) *
                     static_cast<size_t>(video.width) *
                     static_cast<size_t>(video.height);
  video.data.resize(video.noFrames);
  for (long f = 0; f < video.noFrames; ++f) {
    video.data[f].resize(frameSize);
    binVideo.read(reinterpret_cast<char*>(video.data[f].data()), frameSize);
  }

  binVideo.close();

  // printVideo2(video);

  return true;
}

bool readbin2S(const string& filename, Video2& video) {
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

  size_t frameSize = static_cast<size_t>(video.channels) *
                     static_cast<size_t>(video.width) *
                     static_cast<size_t>(video.height);
  size_t totalSize = static_cast<size_t>(video.noFrames) * frameSize;

  vector<unsigned char> Alldata(totalSize);
  binVideo.read(reinterpret_cast<char*>(Alldata.data()), totalSize);
  binVideo.close();

  video.data.resize(video.noFrames);
  for (long f = 0; f < video.noFrames; ++f) {
    video.data[f] = vector<unsigned char>(
        Alldata.begin() + f * frameSize, Alldata.begin() + (f + 1) * frameSize);
  }

  return true;
}

bool readbin3(const string& filename, Video3& video) {
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

  size_t channelSize =
      static_cast<size_t>(video.width) * static_cast<size_t>(video.height);

  video.data.resize(video.noFrames);
  for (long f = 0; f < video.noFrames; ++f) {
    video.data[f].resize(video.channels);
    for (int c = 0; c < video.channels; ++c) {
      video.data[f][c].resize(channelSize);
      binVideo.read(reinterpret_cast<char*>(video.data[f][c].data()),
                    channelSize);
    }
  }

  // printVideo3(video);
  binVideo.close();
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

  binVideo.write(reinterpret_cast<const char*>(&video.noFrames),
                 sizeof(video.noFrames));
  binVideo.write(reinterpret_cast<const char*>(&video.channels),
                 sizeof(video.channels));
  binVideo.write(reinterpret_cast<const char*>(&video.width),
                 sizeof(video.width));
  binVideo.write(reinterpret_cast<const char*>(&video.height),
                 sizeof(video.height));

  if (mode == Video::Mode1D) {
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
  }

  binVideo.close();
  return true;
}

bool writebin1(const string& filename, const Video1& video) {
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
  binVideo.write(reinterpret_cast<const char*>(video.data.data()),
                 video.data.size());

  binVideo.close();

  // printVideo1(video);

  return true;
}

bool writebin2(const string& filename, const Video2& video) {
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
  for (long f = 0; f < video.noFrames; ++f) {
    binVideo.write(reinterpret_cast<const char*>(video.data[f].data()),
                   video.data[f].size());
  }

  binVideo.close();

  return true;
}

bool writebin2S(const string& filename, const Video2& video) {
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

  size_t frameSize = static_cast<size_t>(video.channels) *
                     static_cast<size_t>(video.width) *
                     static_cast<size_t>(video.height);

  size_t totalSize = static_cast<size_t>(video.noFrames) * frameSize;
  vector<unsigned char> alldata(totalSize);
  for (long f = 0; f < video.noFrames; ++f) {
    memcpy(&alldata[f * frameSize], video.data[f].data(), frameSize);
  }

  binVideo.write(reinterpret_cast<const char*>(alldata.data()), totalSize);

  binVideo.close();
  return true;
}

bool writebin3(const string& filename, const Video3& video) {
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
  for (long f = 0; f < video.noFrames; ++f) {
    for (int c = 0; c < video.channels; ++c) {
      binVideo.write(reinterpret_cast<const char*>(video.data[f][c].data()),
                     video.data[f][c].size());
    }
  }

  binVideo.close();

  // printVideo3(video);

  return true;
}

bool reverse(const string& input, const string& output, const string& mode) {
  if (mode == "-S") {
    // use cpp reverse on 2d
    Video video;
    if (!readbinS(input, video, Video::Mode2D)) {
      return false;
    }
    std::reverse(video.data2.begin(), video.data2.end());
    if (!writebinS(output, video, Video::Mode2D)) {
      return false;
    }

    // // use cpp reverse on 2d
    // Video2 video;
    // if (!readbin2S(input, video)) {
    //   return false;
    // }
    // std::reverse(video.data.begin(), video.data.end());
    // if (!writebin2S(output, video)) {
    //   return false;
    // }

    // 1d solution with pointers

    // Video1 video;

    // readbin1(input, video);

    // const size_t frameSize = static_cast<size_t>(video.channels) *
    //                          static_cast<size_t>(video.width) *
    //                          static_cast<size_t>(video.height);

    // for (int i = 0; i < video.noFrames / 2; ++i) {
    //   const long j = video.noFrames - i - 1;

    //   unsigned char* start = video.data.data() + i * frameSize;

    //   unsigned char* end = video.data.data() + j * frameSize;

    //   for (size_t k = 0; k < frameSize; ++k) {
    //     std::swap(start[k], end[k]);
    //   }
    // }

    // writebin1(output, video);
    //
    //
    //// 1d sol copy from back of source into new one
    // Video1 video;
    // if (!readbin1(input, video)) {
    //   return false;
    // }

    // size_t frameSize = (size_t)video.channels * video.width * video.height;
    // std::vector<unsigned char> reversedData(video.data.size());

    // for (long i = 0; i < video.noFrames; ++i) {
    //   long j = video.noFrames - 1 - i;
    //   const unsigned char* src = &video.data[i * frameSize];
    //   unsigned char* dst = &reversedData[j * frameSize];

    //   std::memcpy(dst, src, frameSize);
    // }

    // video.data = reversedData;

    // if (!writebin1(output, video)) {
    //   return false;
    // }
  } else if (mode == "-M") {
    // swap inner and outer frames in 1d array
    // Video1 video;
    // if (!readbin1(input, video)) {
    //   cerr << "Failed to read the input video file." << endl;
    //   return false;
    // }
    // size_t frameSize = static_cast<size_t>(video.channels) *
    //                    static_cast<size_t>(video.width) *
    //                    static_cast<size_t>(video.height);

    // long start = 0;
    // long end = video.noFrames - 1;

    // while (start < end) {
    //   size_t startOffset = static_cast<size_t>(start) * frameSize;
    //   size_t endOffset = static_cast<size_t>(end) * frameSize;

    //   for (size_t i = 0; i < frameSize; ++i) {
    //     swap(video.data[startOffset + i], video.data[endOffset + i]);
    //   }

    //   ++start;
    //   --end;
    // }

    // writebin1(output, video);

    // one chunk per time solution

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
  } else if (mode == "-V") {
    // // do path from top to bottom and copy to new vector
    // Video1 video;
    // if (!readbin1(input, video)) {
    //   std::cerr << "Failed to read the input video file." << std::endl;
    //   return false;
    // }

    // // calculate frame size
    // size_t frameSize = static_cast<size_t>(video.channels) *
    //                    static_cast<size_t>(video.width) *
    //                    static_cast<size_t>(video.height);

    // if (video.data.size() % frameSize != 0) {
    //   std::cerr << "Error: Invalid frame size. Not divisible" <<
    //   std::endl; return false;
    // }

    // int numFrames = video.noFrames;
    // std::vector<int> reversedVid(video.data.size());

    // for (int i = 0; i < numFrames; ++i) {
    //   int startIdxOriginal = i * frameSize;
    //   int startIdxReversed = (numFrames - 1 - i) * frameSize;

    //   std::copy(video.data.begin() + startIdxOriginal,
    //             video.data.begin() + startIdxOriginal + frameSize,
    //             reversedVid.begin() + startIdxReversed);

    //   // now write the reversed video to the output file
    // }

    // use cpp reverse on 2d
    Video video;
    if (!readbin(input, video, Video::Mode2D)) {
      return false;
    }
    std::reverse(video.data2.begin(), video.data2.end());
    if (!writebin(output, video, Video::Mode2D)) {
      return false;
    }
  }
  return true;
}

bool swap_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel1, unsigned char channel2) {
  if (mode == "-S") {
    Video3 video;
    if (!readbin3(input, video)) {
      return false;
    }

    if (channel1 > video.channels || channel2 > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      std::swap(video.data[f][channel1 - 1], video.data[f][channel2 - 1]);
    }

    if (!writebin3(output, video)) {
      return false;
    }
  } else if (mode == "-M") {
    // ifstream video(input, ios::binary);
    // if (!video.is_open()) {
    //   cerr << "Failed to open the input video file." << endl;
    // }

    // long noFrames;
    // unsigned char channels;
    // unsigned char height;
    // unsigned char width;

    // video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    // video.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    // video.read(reinterpret_cast<char*>(&height), sizeof(height));
    // video.read(reinterpret_cast<char*>(&width), sizeof(width));

    // ofstream swappedVideo(output, ios::binary);

    // swappedVideo.write(reinterpret_cast<char*>(&noFrames),
    // sizeof(noFrames));
    // swappedVideo.write(reinterpret_cast<char*>(&channels),
    // sizeof(channels));
    // swappedVideo.write(reinterpret_cast<char*>(&height), sizeof(height));
    // swappedVideo.write(reinterpret_cast<char*>(&width), sizeof(width));

    // size_t frameSize = static_cast<size_t>(channels) *
    //                    static_cast<size_t>(width) *
    //                    static_cast<size_t>(height);

    // size_t channelSize =
    //     static_cast<size_t>(width) * static_cast<size_t>(height);

    // vector<char> frameBuffer(frameSize);

    // // I don't like this iterator tho
    // for (long i = 0; i < noFrames; ++i) {
    //   video.read(frameBuffer.data(), frameSize);
    //   for (size_t j = 0; j < channelSize; ++j) {
    //     size_t pixelOffset1 = channelSize * (channel1 - 1) + j;
    //     size_t pixelOffset2 = channelSize * (channel2 - 1) + j;
    //     std::swap(frameBuffer[pixelOffset1], frameBuffer[pixelOffset2]);
    //   }
    //   swappedVideo.write(frameBuffer.data(), frameSize);
    // }

    // video.close();
    // swappedVideo.close();

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

    // // I don't like this iterator tho
    // for (long i = 0; i < noFrames; ++i) {
    //   video.read(frameBuffer.data(), frameSize);
    //   for (size_t j = 0; j < channelSize; ++j) {
    //     size_t pixelOffset1 = channelSize * (channel1 - 1) + j;
    //     size_t pixelOffset2 = channelSize * (channel2 - 1) + j;
    //     std::swap(frameBuffer[pixelOffset1], frameBuffer[pixelOffset2]);
    //   }
    //   swappedVideo.write(frameBuffer.data(), frameSize);
    // }

  } else if (mode == "-V") {
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

    // // I don't like this iterator tho
    // for (long i = 0; i < noFrames; ++i) {
    //   video.read(frameBuffer.data(), frameSize);
    //   for (size_t j = 0; j < channelSize; ++j) {
    //     size_t pixelOffset1 = channelSize * (channel1 - 1) + j;
    //     size_t pixelOffset2 = channelSize * (channel2 - 1) + j;
    //     std::swap(frameBuffer[pixelOffset1], frameBuffer[pixelOffset2]);
    //   }
    //   swappedVideo.write(frameBuffer.data(), frameSize);
    // }
  }
  return true;
}

bool clip_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel, unsigned char min, unsigned char max) {
  if (mode == "-S") {
    Video3 video;
    if (!readbin3(input, video)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        video.data[f][channel - 1][i] =
            std::max(min, std::min(max, video.data[f][channel - 1][i]));
        // cout << static_cast<int>(video.data[f][channel - 1][i]) << " ";
      }
    }
    if (!writebin3(output, video)) {
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
      // print all the values of frameBuffer
      // for (size_t k = 0; k < frameSize; ++k) {
      //   cout << static_cast<int>(frameBuffer[k]) << " ";
      // }

      clippedVideo.write(reinterpret_cast<char*>(frameBuffer.data()),
                         frameSize);
    }

    video.close();
    clippedVideo.close();

  } else if (mode == "-V") {
    Video3 video;
    if (!readbin3(input, video)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data[f][channel - 1][i] < min) {
          video.data[f][channel - 1][i] = min;
        } else if (video.data[f][channel - 1][i] > max) {
          video.data[f][channel - 1][i] = max;
        }
      }
    }
    if (!writebin3(output, video)) {
      return false;
    }
  }
  return true;
}

bool scale_channel(const string& input, const string& output,
                   const string& mode, unsigned char channel, float factor) {
  if (mode == "-S") {
    Video3 video;
    if (!readbin3(input, video)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data[f][channel - 1][i] != 0) {
          if (factor * video.data[f][channel - 1][i] > 255) {
            video.data[f][channel - 1][i] = 255;
          } else if (factor <= 0) {
            video.data[f][channel - 1][i] = 0;
          }

          else {
            video.data[f][channel - 1][i] =
                video.data[f][channel - 1][i] * factor;
          }
        }
      }
    }

    if (!writebin3(output, video)) {
      return false;
    }
  } else if (mode == "-M") {
    // Video3 video;
    // readbin3(input, video);

    // for (long f = 0; f < video.noFrames; ++f) {
    //   for (size_t i = 0; i < video.width * video.height; ++i) {
    //     video.data[f][channel - 1][i] = video.data[f][channel - 1][i] *
    //     factor;
    //   }
    // }
    // writebin3(output, video);
    Video3 video;
    if (!readbin3(input, video)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data[f][channel - 1][i] != 0) {
          if (factor * video.data[f][channel - 1][i] > 255) {
            video.data[f][channel - 1][i] = 255;
          } else if (factor <= 0) {
            video.data[f][channel - 1][i] = 0;
          }

          else {
            video.data[f][channel - 1][i] =
                video.data[f][channel - 1][i] * factor;
          }
        }
      }
    }

    if (!writebin3(output, video)) {
      return false;
    }
  } else if (mode == "-V") {
    Video3 video;
    if (!readbin3(input, video)) {
      return false;
    }

    if (channel > video.channels) {
      cerr << "Error: Invalid channel number." << endl;
      return false;
    }

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data[f][channel - 1][i] != 0) {
          if (factor * video.data[f][channel - 1][i] > 255) {
            video.data[f][channel - 1][i] = 255;
          } else if (factor <= 0) {
            video.data[f][channel - 1][i] = 0;
          }

          else {
            video.data[f][channel - 1][i] =
                video.data[f][channel - 1][i] * factor;
          }
        }
      }
    }

    if (!writebin3(output, video)) {
      return false;
    }
  }
  return true;
}
