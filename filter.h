#pragma once

#include "DaisyDuino.h"

namespace synthux {

class Filter {
public:
  void Init(float sampleRate) {
    _flt.Init(sampleRate);
  }

  void SetRange(float low, float high) {
    _low_freq = low;
    _high_freq = high;
  }

  void SetCutoff(float timbre) {
    auto fltFreq = _map(timbre, _low_freq, _high_freq);
    _flt.SetFreq(fltFreq);
  }

  void SetReso(float reso) {
    _flt.SetRes(_map(reso, 0.f, .9f)); //Don't set higher than 0.9. It's unstable.
  }

  float Process(const float in) {
    return _flt.Process(in);
  }

private:
  daisysp::MoogLadder _flt;

  float _map(float val, float min, float max) {
    return (max - min) * val + min;
  }

  float _low_freq = 80.f; //Hz
  float _high_freq = 4000.f; //Hz
};

};
