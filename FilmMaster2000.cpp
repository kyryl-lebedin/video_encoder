#include "FilmMaster2000.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

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
    // // use cpp reverse on 2d
    // Video2 video;
    // if (!readbin2(input, video)) {
    //   cerr << "Failed to read the input video file." << endl;
    //   return false;
    // }

    // std::reverse(video.data.begin(), video.data.end());
    // writebin2(output, video);

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

    Video1 video;
    readbin1(input, video);
    size_t frameSize = (size_t)video.channels * video.width * video.height;
    std::vector<unsigned char> reversedData(video.data.size());
    for (long i = 0; i < video.noFrames; ++i) {
      long j = video.noFrames - 1 - i;
      const unsigned char* src = &video.data[i * frameSize];
      unsigned char* dst = &reversedData[j * frameSize];

      std::memcpy(dst, src, frameSize);
    }

    video.data = reversedData;
    writebin1(output, video);

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
      cerr << "Failed to open the input video file." << endl;
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
      cerr << "Failed to open the output video file." << endl;
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
    //   std::cerr << "Error: Invalid frame size. Not divisible" << std::endl;
    //   return false;
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
    Video2 video;
    if (!readbin2(input, video)) {
      cerr << "Failed to read the input video file." << endl;
      return false;
    }

    std::reverse(video.data.begin(), video.data.end());
    writebin2(output, video);
  }
  return true;
}

void swap_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel1, unsigned char channel2) {
  if (mode == "-S") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      std::swap(video.data[f][channel1 - 1], video.data[f][channel2 - 1]);
    }
    writebin3(output, video);
  }

  else if (mode == "-M") {
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

    // swappedVideo.write(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    // swappedVideo.write(reinterpret_cast<char*>(&channels), sizeof(channels));
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
      cerr << "Failed to open the input video file." << endl;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    ofstream swappedVideo(output, ios::binary);

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

  else if (mode == "-V") {
    ifstream video(input, ios::binary);
    if (!video.is_open()) {
      cerr << "Failed to open the input video file." << endl;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    ofstream swappedVideo(output, ios::binary);

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
}

void clip_channel(const string& input, const string& output, const string& mode,
                  unsigned char channel, unsigned char min, unsigned char max) {
  if (mode == "-S") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        video.data[f][channel - 1][i] =
            std::max(min, std::min(max, video.data[f][channel - 1][i]));
        // cout << static_cast<int>(video.data[f][channel - 1][i]) << " ";
      }
    }
    writebin3(output, video);
  } else if (mode == "-M") {
    ifstream video(input, ios::binary);
    if (!video.is_open()) {
      cerr << "Failed to open the input video file." << endl;
    }

    long noFrames;
    unsigned char channels;
    unsigned char height;
    unsigned char width;

    video.read(reinterpret_cast<char*>(&noFrames), sizeof(noFrames));
    video.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    video.read(reinterpret_cast<char*>(&height), sizeof(height));
    video.read(reinterpret_cast<char*>(&width), sizeof(width));

    ofstream clippedVideo(output, ios::binary);

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
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data[f][channel - 1][i] < min) {
          video.data[f][channel - 1][i] = min;
        } else if (video.data[f][channel - 1][i] > max) {
          video.data[f][channel - 1][i] = max;
        }
      }
    }
    writebin3(output, video);
  }
}

void scale_channel(const string& input, const string& output,
                   const string& mode, unsigned char channel, float factor) {
  if (mode == "-S") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        if (video.data[f][channel - 1][i] != 0) {
          if (factor * video.data[f][channel - 1][i] > 255) {
            video.data[f][channel - 1][i] = 255;
          } else {
            video.data[f][channel - 1][i] =
                video.data[f][channel - 1][i] * factor;
          }
        }
      }
    }
    writebin3(output, video);
  } else if (mode == "-M") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        video.data[f][channel - 1][i] = video.data[f][channel - 1][i] * factor;
      }
    }
    writebin3(output, video);
  } else if (mode == "-V") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        video.data[f][channel - 1][i] = video.data[f][channel - 1][i] * factor;
      }
    }
    writebin3(output, video);
  }
}
