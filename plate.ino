// SYNTHUX ACADEMY /////////////////////////////////////////
// TEMPLATE PROJECT FOR SIMPLE TOUCH BOARD AND DAISY SEED //

#include "DaisyDuino.h"
#include "aknob.h"
#include "filter.h"

using namespace synthux;

//////////////////////////////////////////////////////////////
////////////////////// PARAMETERS ////////////////////////////
const float delay_feedback = 0.7;       // 0...1
const float filter_low_freq = 80.0;     // Hz
const float filter_high_freq = 4000.0;  // Hz
const float compressor_ratio = 10.0;     // 1...40
const float compressor_low_attack = 0.002;  // 0.001...10
const float compressor_high_attack = 2.0;  // 0.001...10
const float compressor_release = 0.1;   // 0.001...10

//////////////////////////////////////////////////////////////
////////////////////// KNOBS & SWITCHES //////////////////////
static AKnob tresh_knob(A0);
static AKnob attack_knob(A1);
static AKnob cutoff_knob(A2);
static AKnob reso_knob(A3);
static AKnob time_knob(A5);
static AKnob wet_knob(A6);
static AKnob chorus_knob(A7);

//////////////////////////////////////////////////////////////
///////////////////////// MODULES ////////////////////////////
static Compressor comp;
static Filter filter;
static Chorus chorus;

const int kDelayBufferLength = 96000; //2s @ 48KHz sample rate. delayLeftTime and kDelayRightTime can not be longer than this.
DelayLine<float, kDelayBufferLength> delay_line;
float delay_wet_mix;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK //////////////////////////

float wet, dry, chorus_mix, output;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // Input ###################################
    dry = in[0][i];

    // Chorus ##################################
    dry = chorus_mix * chorus.Process(dry) + (1.f - chorus_mix) * dry;

    // Filter ##################################
    dry = filter.Process(dry);

    // Delay ###################################
    float wet = delay_line.Read();
    delay_line.Write((wet * delay_feedback) + dry);
    output = delay_wet_mix * wet + (1.f - delay_wet_mix) * dry;
    
    // Compress ################################
    output = comp.Process(output);

    out[0][i] = output;
    out[1][i] = 0;
  }
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////
void setup() {
  // SETUP DAISY
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  auto sample_rate = DAISY.get_samplerate();

  delay_line.Init();
  
  chorus.Init(sample_rate);

  filter.Init(sample_rate);
  filter.SetRange(filter_low_freq, filter_high_freq);

  comp.Init(sample_rate);
  comp.SetRatio(compressor_ratio);
  comp.SetRelease(compressor_release);
  comp.AutoMakeup(false);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

void loop() {
  
  chorus_mix = chorus_knob.Process();

  filter.SetCutoff(cutoff_knob.Process());
  filter.SetReso(reso_knob.Process());

  delay_wet_mix = wet_knob.Process();
  delay_line.SetDelay(fmap(time_knob.Process(), 480.f, 96000.f)); //10ms...2s

  comp.SetThreshold(fmap(tresh_knob.Process(), 0.f, 80.f) - 80.f);
  comp.SetAttack(fmap(attack_knob.Process(), compressor_low_attack, compressor_high_attack, Mapping::EXP));  

  delay(4);
}
