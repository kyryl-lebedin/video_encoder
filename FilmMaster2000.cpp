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
  // for (size_t i = 0; i < video.width * video.height; ++i) {
  //   cout << static_cast<int>(video.data[0][0][i]) << " ";
  // }
  // cout << endl;
  // cout << "Last frame pixel data: ";
  // for (size_t i = video.data[video.noFrames - 1][video.channels - 1].size() -
  //                 video.width * video.height;
  //      i < video.data[video.noFrames - 1][video.channels - 1].size(); ++i) {
  //   cout << static_cast<int>(
  //               video.data[video.noFrames - 1][video.channels - 1][i])
  //        << " ";
  // }
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
    Video1 video;
    if (!readbin1(input, video)) {
      cerr << "Failed to read the input video file." << endl;
      return false;
    }
    size_t frameSize = static_cast<size_t>(video.channels) *
                       static_cast<size_t>(video.width) *
                       static_cast<size_t>(video.height);

    long start = 0;
    long end = video.noFrames - 1;

    while (start < end) {
      size_t startOffset = static_cast<size_t>(start) * frameSize;
      size_t endOffset = static_cast<size_t>(end) * frameSize;

      for (size_t i = 0; i < frameSize; ++i) {
        swap(video.data[startOffset + i], video.data[endOffset + i]);
      }

      ++start;
      --end;
    }

    writebin1(output, video);
  } else if (mode == "-V") {
    // do path from top to bottom and copy to new vector
    Video1 video;
    if (!readbin1(input, video)) {
      std::cerr << "Failed to read the input video file." << std::endl;
      return false;
    }

    // calculate frame size
    size_t frameSize = static_cast<size_t>(video.channels) *
                       static_cast<size_t>(video.width) *
                       static_cast<size_t>(video.height);

    if (video.data.size() % frameSize != 0) {
      std::cerr << "Error: Invalid frame size. Not divisible" << std::endl;
      return false;
    }

    int numFrames = video.noFrames;
    std::vector<int> reversedVid(video.data.size());

    for (int i = 0; i < numFrames; ++i) {
      int startIdxOriginal = i * frameSize;
      int startIdxReversed = (numFrames - 1 - i) * frameSize;

      std::copy(video.data.begin() + startIdxOriginal,
                video.data.begin() + startIdxOriginal + frameSize,
                reversedVid.begin() + startIdxReversed);

      // now write the reversed video to the output file
    }
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
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      std::swap(video.data[f][channel1 - 1], video.data[f][channel2 - 1]);
    }
    writebin3(output, video);
  }

  else if (mode == "-V") {
    // Video1 video;
    // readbin1(input, video);

    // size_t frameSize = static_cast<size_t>(video.channels)
    //                  * static_cast<size_t>(video.width)
    //                  * static_cast<size_t>(video.height);

    // size_t channelSize = static_cast<size_t>(video.width)
    //                    * static_cast<size_t>(video.height);

    // for (long f = 0; f < video.noFrames; ++f) {
    //     size_t frameOffset = f * frameSize;
    //     size_t channelOffset1 = static_cast<size_t>(channel1) * channelSize
    //     + frameOffset; size_t channelOffset2 =
    //     static_cast<size_t>(channel2) * channelSize + frameOffset;

    //     for (size_t i = 0; i < channelSize; ++i) {
    //         swap(video.data[channelOffset1 + i], video.data[channelOffset2
    //         + i]);
    //     }
    // writebin1(output, video);
    // }

    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      std::swap(video.data[f][channel1 - 1], video.data[f][channel2 - 1]);
    }

    writebin3(output, video);
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
            std::max(min, std::min(max, video.data[f][channel][i]));
      }
    }
    writebin3(output, video);
  } else if (mode == "-M") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        video.data[f][channel - 1][i] =
            std::max(min, std::min(max, video.data[f][channel][i]));
      }
    }
    writebin3(output, video);
  } else if (mode == "-V") {
    Video3 video;
    readbin3(input, video);

    for (long f = 0; f < video.noFrames; ++f) {
      for (size_t i = 0; i < video.width * video.height; ++i) {
        video.data[f][channel - 1][i] =
            std::max(min, std::min(max, video.data[f][channel][i]));
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
        video.data[f][channel - 1][i] = video.data[f][channel - 1][i] * factor;
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
