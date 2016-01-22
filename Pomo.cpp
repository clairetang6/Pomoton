/*
Copyright 2016 Claire Tang

Pomodoro app for the Particle Photon InternetButton

To start a session, press the bottom button (#2).
Tomato red LEDs count down each session, and when
the session is over, a rainbow and tune plays.
Press the top button (#0) while the session is going
to switch into distraction-free mode.
Finally, track your sessions completed by pressing
the right button (#1).
*/

#include "application.h"
#include "BetterPhotonButton.h"

#define POMO_RUNNING 0
#define POMO_COUNT 1
#define POMO_TIMER 3
#define POMO_START 2

#define POMO_DURATION 60*1000*1 //duration of each session in ms.
                                //POMO_DURATION must be greater than RAINBOW_DURATION
#define RAINBOW_DURATION 2000

BetterPhotonButton bb = BetterPhotonButton();

int current_state = -1;
bool running = false;
int count = 0;
int timer_leds_on = PIXEL_COUNT;
int start_time = 0;
int end_time = 0;
PixelColor white = PixelColor::WHITE;
PixelColor red = PixelColor::RED;

void playTune(){
  Serial.printlnf("playing tune");
  bb.playNotes(":8,C-,E-,G-,C,G:4");
  bb.rainbow(250, RAINBOW_DURATION);
}

void startPomo(int current_time){
  start_time = current_time;
  running = true;
  timer_leds_on = PIXEL_COUNT;
}

void updatePomo(int current_time){
  int duration_passed = current_time - start_time;
  float duration_left = POMO_DURATION - duration_passed;
  if(duration_left > 0){
    timer_leds_on = (int)(11 * (duration_left/(POMO_DURATION+1)) + 1);
  }else{
    running = false;
    end_time = current_time;
    count++;
    current_state = POMO_COUNT;
    playTune();
  }
}

void showTimer(){
  bb.setPixels(0);
  for(int i = 0; i < timer_leds_on; i++){
    bb.setPixel(i, red);
  }
}

void showCount(){
  bb.setPixels(0);
  for(int i = 0; i < count; i++){
    bb.setPixel(i, white);
  }
}

void buttonPressedHandler(int button, bool state){
  Serial.printlnf("button pressed: %d", button);
  switch(button){
    case POMO_RUNNING:
      if(running){
        bb.startPixelAnimation(&animation_fader, &paletteRainbow, 2000);
      }else{
        bb.setPixels(0);
      }
      current_state = POMO_RUNNING;
      break;
    case POMO_COUNT:
      current_state = POMO_COUNT;
      break;
    case POMO_START:
      if(!running){
        startPomo(millis());
      }
      current_state = POMO_TIMER;
      break;
    case POMO_TIMER:
      current_state = POMO_TIMER;
      break;
  }
}

void setup() {
  bb.setup();
  bb.rainbow(250, 1000);
  bb.setPressedHandler(&buttonPressedHandler);
}

void loop(){
  int current = millis();

  bb.update(current);
  if(running){
    updatePomo(current);
    if(current%100 == 0){
      Serial.printlnf("timer leds %d", timer_leds_on);
    }
  }

  switch(current_state){
    case POMO_RUNNING:
      if(!running){
        bb.setPixels(0);
      }
      break;
    case POMO_COUNT:
      if(!(current >= end_time && current < end_time + RAINBOW_DURATION)){
        showCount();
      }
      break;
    case POMO_TIMER:
      showTimer();
      break;
  }
}
