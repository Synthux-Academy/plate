// SYNTHUX ACADEMY /////////////////////////////////////////
// TEMPLATE PROJECT FOR SIMPLE TOUCH BOARD AND DAISY SEED //

#include "simple-touch-daisy.h"
#include "aknob.h"
// #include "vox.h"
#include "follow.h"
#include "filter.h"

using namespace synthux;

//////////////////////////////////////////////////////////////
///////////////////////// MODULES ////////////////////////////
// static Vox vox;
static Chorus chorus;
static Follower follow(0.4, 0.999);
static Filter filter;

//////////////////////////////////////////////////////////////
////////////////////// KNOBS & SWITCHES //////////////////////
static AKnob input_knob(A(S30));
static AKnob cutoff_knob(A(S32));
static AKnob reso_knob(A(S33));
static AKnob delay_knob(A(S34));
static AKnob wet_knob(A(S35));
static AKnob chorus_fader(A(S36));
static AKnob chorus_mix_fader(A(S37));

static const int drone_switch = D(S07);
static const int switch_1_b = D(S08);
static const int switch_2_a = D(S09);
static const int follow_switch = D(S10);

//////////////////////////////////////////////////////////////
/////////////////////////// TOUCH  ///////////////////////////
static synthux::simpletouch::Touch touch;

float decay = 1.f;

void OnTouch(uint16_t pad) {
  // if (pad == 4) vox.Trigger(decay);
}

void OnRelease(uint16_t pad) {

}

///////////////////////////////////////////////////////////////
////////////////////// DELAY LINE /////////////////////////////
const int kDelayBufferLength = 96000; //2s @ 48KHz sample rate. delayLeftTime and kDelayRightTime can not be longer than this.
DelayLine<float, kDelayBufferLength> delay_line;
float delayFeedback = 0.7; // 0...1
float delayWetMix = 0.33; // 0...1

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK //////////////////////////

float inputGain = 1.f;
float chorusMix = 1.f;
bool feedbackOn = false;

void AudioCallback(float **in, float **out, size_t size) {
  float oscout = 0;
  float delayout = 0;
  for (size_t i = 0; i < size; i++) {
    // oscout = vox.Process();

    // Delay ##################################
    float dry = in[0][i] * inputGain;
    dry = chorus.Process(dry) * chorusMix + (1.f - chorusMix) * dry;
    dry = filter.Process(dry);

    float wet = delay_line.Read();
    delay_line.Write((wet * delayFeedback) + dry);
    delayout = wet * delayWetMix + dry * (1 - delayWetMix);
    // if (feedbackOn) {
    //   oscout *= follow.Process(delayout);
    // }
    out[0][i] = delayout;
    out[1][i] = oscout;
  }
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////
void setup() {
  // SETUP DAISY
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  auto sample_rate = DAISY.get_samplerate();
  
  Serial.begin(9600);

  // INIT TOUCH SENSOR
  touch.Init();
  touch.SetOnTouch(OnTouch);
  touch.SetOnRelease(OnRelease);

  // INIT SWITCHES
  pinMode(drone_switch, INPUT_PULLUP);
  pinMode(switch_1_b, INPUT_PULLUP);
  pinMode(switch_2_a, INPUT_PULLUP);
  pinMode(follow_switch, INPUT_PULLUP);

  // initialize other modules here
  // vox.Init(sample_rate);
  delay_line.Init();
  chorus.Init(sample_rate);
  filter.Init(sample_rate);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

void loop() {
  //PROCESS TOUCH SENSOR
  touch.Process();

  // auto newFeedbackOn = !digitalRead(follow_switch);
  // if (!feedbackOn && newFeedbackOn) vox.Trigger(decay);
  // feedbackOn = newFeedbackOn;
  
  // vox.SetDroning(feedbackOn || digitalRead(drone_switch));
  // vox.SetFreq(fmap(freq_knob.Process(), 30.0, 5000, Mapping::LOG));
  // decay = fmap(decay_knob.Process(), 0.01, 3.0);

  inputGain = fmap(input_knob.Process(), 0.2, 10.0, Mapping::EXP);
  delayWetMix = wet_knob.Process();
  delay_line.SetDelay(fmap(delay_knob.Process(), 480, 96000));

  auto chorus_val = chorus_fader.Process();
  chorus.SetLfoFreq(chorus_val);
  chorus.SetLfoDepth(1.f - chorus_val);
  chorus.SetDelay(1.f - fmap(chorus_val, 0.1, 0.9));
  chorusMix = chorus_mix_fader.Process();

  filter.SetCutoff(cutoff_knob.Process());
  filter.SetReso(reso_knob.Process());

  delay(4);
}
