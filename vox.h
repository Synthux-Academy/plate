#pragma once

#include "DaisyDuino.h"

namespace synthux {

class Vox {
public:
void Init(float sample_rate) {
  // OSCILLATOR SETUP
  _osc.Init(sample_rate);
  _osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SQUARE);

  //ENV SETUP
  _env.Init(sample_rate);
  _env.SetTime(ADSR_SEG_ATTACK, .0);
}

void Trigger(float decay) {
  _env.SetTime(ADSR_SEG_RELEASE, decay);
  _osc.Reset();
  _OpenGate();
}

void SetAmp(float amp) {
  _osc_amp = amp;
}

void SetFreq(float freq) {
  _osc_freq = freq;
}

void SetDroning(bool value) {
  _sustain = value;
}

float Process() { 
    auto env_amp = _env.Process(_gate);
    if (!_sustain) _CloseGate();
    if (!_env.IsRunning()) return 0;
    _osc.SetFreq(_osc_freq);
    _osc.SetAmp(env_amp * _osc_amp);
    return _osc.Process();
}

private:
  void _OpenGate() {
    _gate = true;
  }

  void _CloseGate() {
    _gate = false;
  }

  bool _gate = false;
  bool _sustain = false;
  float _osc_freq;
  float _osc_amp = 1.0;
  float _decay = 0.02;
  Oscillator _osc;
  Adsr _env;
};
};
