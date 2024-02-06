// SYNTHUX ACADEMY /////////////////////////////////////////
// TEMPLATE PROJECT FOR SIMPLE TOUCH BOARD AND DAISY SEED //

#include "simple-touch-daisy.h"
#include "aknob.h"
#include "filter.h"

using namespace synthux;

//////////////////////////////////////////////////////////////
////////////////////// PARAMETERS ////////////////////////////
const float delay_feedback = 0.7;       // 0...1
const float filter_low_freq = 80.0;     // Hz
const float filter_high_freq = 4000.0;  // Hz
const float compressor_ratio = 10.0;     // 1...40
const float compressor_attack = 0.005;  // 0.001...10 
const float compressor_release = 0.1;   // 0.001...10

//////////////////////////////////////////////////////////////
////////////////////// KNOBS & SWITCHES //////////////////////
//
//    |-| (*)   (*)   (*)    (*) |-|
//    | | S31   S32   S33    S34 | |
//    |||                        |||
//    |_| (*)                (*) |_|
//    S36 S30                S35 S37

static AKnob cutoff_knob(A(S30));
static AKnob reso_knob(A(S31));
static AKnob tresh_knob(A(S32));
static AKnob time_knob(A(S34));
static AKnob wet_knob(A(S35));
static AKnob input_fader(A(S36));
static AKnob gain_fader(A(S37));

//////////////////////////////////////////////////////////////
/////////////////////////// TOUCH  ///////////////////////////
// static synthux::simpletouch::Touch touch;
// void OnTouch(uint16_t pad) {}
// void OnRelease(uint16_t pad) {}

//////////////////////////////////////////////////////////////
///////////////////////// MODULES ////////////////////////////
static Compressor comp;
static Filter filter;

const int kDelayBufferLength = 96000; //2s @ 48KHz sample rate. delayLeftTime and kDelayRightTime can not be longer than this.
DelayLine<float, kDelayBufferLength> delay_line;
float delay_wet_mix;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK //////////////////////////

float in_gain = 1.f;
float wet, dry, output;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // Input ##################################
    dry = in[0][i] * in_gain;

    // Filter ##################################
    dry = filter.Process(dry);

    // Delay ##################################
    float wet = delay_line.Read();
    delay_line.Write((wet * delay_feedback) + dry);
    output = wet * delay_wet_mix + dry * (1.f - delay_wet_mix);
    
    // Compress ##################################
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
  
  // Serial.begin(9600);

  // INIT TOUCH SENSOR
  // touch.Init();
  // touch.SetOnTouch(OnTouch);
  // touch.SetOnRelease(OnRelease);

  // initialize other modules here
  delay_line.Init();
  
  filter.Init(sample_rate);
  filter.SetRange(filter_low_freq, filter_high_freq);

  comp.Init(sample_rate);
  comp.SetRatio(compressor_ratio);
  comp.SetAttack(compressor_attack);
  comp.SetRelease(compressor_release);
  comp.AutoMakeup(false);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

void loop() {
  // touch.Process();

  in_gain = fmap(input_fader.Process(), .2f, 10.f, Mapping::EXP);
  
  delay_wet_mix = wet_knob.Process();
  delay_line.SetDelay(fmap(time_knob.Process(), 480.f, 96000.f)); //10ms...2s

  filter.SetCutoff(cutoff_knob.Process());
  filter.SetReso(reso_knob.Process());

  comp.SetThreshold(fmap(tresh_knob.Process(), 0.f, 80.f) - 80.f);
  comp.SetMakeup(fmap(gain_fader.Process(), 0.f, 50.f));

  delay(4);
}
