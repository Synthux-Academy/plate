// SYNTHUX ACADEMY /////////////////////////////////////////
// TEMPLATE PROJECT FOR SIMPLE TOUCH BOARD AND DAISY SEED //

#include "simple-touch-daisy.h"
#include "aknob.h"
#include "vox.h"

using namespace synthux;

//////////////////////////////////////////////////////////////
///////////////////////// MODULES ////////////////////////////
static Vox vox;

//////////////////////////////////////////////////////////////
////////////////////// KNOBS & SWITCHES //////////////////////
static AKnob input_knob(A(S30));
static AKnob decay_knob(A(S32));
static AKnob freq_knob(A(S33));
static AKnob knob_e(A(S34));
static AKnob knob_f(A(S35));
static AKnob left_fader(A(S36));
static AKnob right_fader(A(S37));

static const int drone_switch = D(S07);
static const int switch_1_b = D(S08);
static const int switch_2_a = D(S09);
static const int switch_2_b = D(S10);

//////////////////////////////////////////////////////////////
/////////////////////////// TOUCH  ///////////////////////////
static synthux::simpletouch::Touch touch;

float amp = 1.0;
float decay = 1.f;

void OnTouch(uint16_t pad) {
  if (pad == 4) vox.Trigger(decay);
}

void OnRelease(uint16_t pad) {

}

///////////////////////////////////////////////////////////////
////////////////////// DELAY LINE /////////////////////////////
const int kDelayBufferLength = 96000; //2s @ 48KHz sample rate. delayLeftTime and kDelayRightTime can not be longer than this.
DelayLine<float, kDelayBufferLength> delay_line;

float delayTime = 12000; //250ms @ 48KHz sample rate. Should not be longer than kDelayBufferLength
float delayFeedback = 0.8; // 0...1
float delayWetMix = 0.33; // 0...1

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK //////////////////////////
void AudioCallback(float **in, float **out, size_t size) {
  float oscout = 0;
  float delayout = 0;
  for (size_t i = 0; i < size; i++) {
    oscout = vox.Process();

    // Delay ##################################
    float dry = in[0][i];
    float wet = delay_line.Read();
    delay_line.Write((wet * delayFeedback) + dry);
    delayout = wet * delayWetMix + dry * (1 - delayWetMix);
    out[0][i] = oscout;
    out[1][i] = delayout;
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
  pinMode(switch_2_b, INPUT_PULLUP);

  // initialize other modules here
  vox.Init(sample_rate);
  delay_line.Init();
  delay_line.SetDelay(delayTime);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

void loop() {
  //PROCESS TOUCH SENSOR
  touch.Process();

  vox.SetDroning(digitalRead(drone_switch));
  vox.SetFreq(fmap(freq_knob.Process(), 30.0, 5000, Mapping::LOG));
  decay = fmap(decay_knob.Process(), 0.01, 3.0);

  //Examine whether particular pad (3 in this case) is being touched
  // auto is_pad_touched = touch.IsTouched(3); //0...11

  //Process knob values 
  // auto knob_a_value = analogRead(knob_a);

  //Process switch values
  //The value of digitalRead is inverted
  // auto switch_a_value = digitalRead(switch_1_a);
  
  delay(4);
}
