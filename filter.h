#pragma once

#include "DaisyDuino.h"

namespace synthux {

class Filter {
public:
  void Init(float sampleRate) {
    _flt.Init(sampleRate);
  }

  void SetCutoff(float timbre) {
    auto fltFreq = _map(timbre, 80.f, 10000.f);
    _flt.SetFreq(fltFreq);
  }

  void SetReso(float reso) {
    _flt.SetRes(_map(reso, 0.f, .9f)); 
  }

  float Process(const float in) {
    return _flt.Process(in);
  }

private:
  daisysp::MoogLadder _flt;

  float _map(float val, float min, float max) {
    return (max - min) * val + min;
  }
};

};
