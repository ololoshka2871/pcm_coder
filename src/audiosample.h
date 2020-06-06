#ifndef AUDIOSAMPLE_H
#define AUDIOSAMPLE_H

template <typename T> union AudioSample {
  struct {
    T L, R;
  };
  T all[2];
};

#endif // AUDIOSAMPLE_H
