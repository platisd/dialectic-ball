#include <Nokia_LCD.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "bitmaps.h"

const uint8_t V_ACC_PIN = 8; // PB2
const uint8_t CLK_PIN = 9; // PB1
const uint8_t V_DIS_PIN = 10; // PB0
const uint8_t X_OUT_PIN = 7; // PA7
const uint8_t Y_OUT_PIN = 6; // PA6
const uint8_t Z_OUT_PIN = 5; // PA5
const uint8_t RST_PIN = 4; // PA4
const uint8_t DIN_PIN = 3; // PA3
const uint8_t CE_PIN = 2; // PA2
const uint8_t DC_PIN = 1; // PA1
const uint8_t LED_PIN = 0; // PA0

// Input & output pins directly controlled in this scope
const uint8_t OUTPUT_PINS[] = {V_ACC_PIN, V_DIS_PIN, LED_PIN};
const uint8_t INPUT_PINS[] = {X_OUT_PIN, Y_OUT_PIN, Z_OUT_PIN};

volatile bool watchdogBarked = false;
Nokia_LCD lcd(CLK_PIN /* CLK */, DIN_PIN /* DIN */, DC_PIN /* DC */, CE_PIN /* CE */, RST_PIN /* RST */);

enum WatchDogTimeout {
  WDT_16ms = 0,
  WDT_32ms,
  WDT_64ms,
  WDT_128ms,
  WDT_250ms,
  WDT_500ms,
  WDT_1sec,
  WDT_2sec,
  WDT_4sec,
  WDT_8sec
};

enum RunningState {
  // Ball facing down for long amount of time
  // Screen is off
  // Slow check of accelerometer for changes
  DEEP_SLEEP,
  // Ball facing down for short amount of time
  // Screen is off
  // Fast check of accelerometer for changes
  WAIT_TO_SLEEP,
  // Ball facing up
  // Screen is on - Waiting display
  // Fast check of accelerometer for changes
  WAIT_TO_PLAY,
  // Ball facing up
  // Screen is on - Game views
  // Faster check of accelerometer for changes
  PLAYING,
};

/**
  Watchdog interrupt routine to be triggered when watchdog times out.
*/
ISR(WDT_vect) {
  watchdogBarked = true;
}

/**
    Sets up watchdog to be triggered (once) after the specified time
    @param wdt  the watchdog timeout duration
*/
void triggerWatchDogIn(WatchDogTimeout wdt) {
  // Adopted from InsideGadgets (www.insidegadgets.com)
  byte timeoutVal = wdt & 7;
  if (wdt > 7) {
    timeoutVal |= (1 << 5);
  }
  timeoutVal |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);
  WDTCSR |= (1 << WDCE) | (1 << WDE); // Start timed sequence
  WDTCSR = timeoutVal;
  WDTCSR |= _BV(WDIE);
  wdt_reset();  // Pat the dog
}

/**
   A utility method to derive a watchdog timeout's duration
   @param wdt the watchdog timeout
   @return    the amount of milliseconds corresponding to a watchdog timeout
*/
unsigned long getTimeoutDuration(WatchDogTimeout wdt) {
  return 1 << (wdt + 4);
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_all_disable ();
  sleep_enable();
  sleep_cpu(); // Sleep here and wait for the interrupt
  sleep_disable();
  power_all_enable(); // power everything back on
}

/**
   Blocks and stays in deep sleep until the specified time has elapsed
   using the current watchdog timeout.
   @param sleepDuration     how long to stay in deep sleep in milliseconds
   @param timeoutInterval   the watchdog timeout interval
*/
void stayInDeepSleepFor(unsigned long sleepDuration, WatchDogTimeout timeoutInterval) {
  unsigned long sleepTime = 0;

  // Start by triggering the watchdog to wake us up every `timeoutInterval`
  triggerWatchDogIn(timeoutInterval);
  while (sleepTime <= sleepDuration) {
    // Sleep until an interrupt occurs (external, change or watchdog)
    goToSleep();
    // Verify we woke up because of the watchdog and not
    // a spurious wake up due to some other unrelated interrupt.
    if (watchdogBarked) {
      // Note down that we have processed the watchdog bark
      watchdogBarked = false;
      // Increase the time we have already slept
      sleepTime += getTimeoutDuration(timeoutInterval);
    }
  }
  wdt_disable(); // Disable watchdog so it stops barking
}

/*** Helper functions ***/
void turnScreenOn() {
  digitalWrite(V_DIS_PIN, HIGH);
}

void turnScreenOff() {
  digitalWrite(V_DIS_PIN, LOW);
}

void turnBacklightOn() {
  digitalWrite(LED_PIN, LOW);
}

void turnBacklightOff() {
  digitalWrite(LED_PIN, HIGH);
}

void turnAccelerometerOn() {
  digitalWrite(V_ACC_PIN, HIGH);
}

void turnAccelerometerOff() {
  digitalWrite(V_ACC_PIN, LOW);
}

unsigned int getAccelerometerX() {
  return analogRead(X_OUT_PIN);
}

unsigned int getAccelerometerY() {
  return analogRead(Y_OUT_PIN);
}

unsigned int getAccelerometerZ() {
  return analogRead(Z_OUT_PIN);
}

void disableADC() {
  ADCSRA &= ~(1 << ADEN);
}

void enableADC() {
  ADCSRA |= (1 << ADEN);
}

/*** Helper functions ***/

void setup() {
  for (auto pin : INPUT_PINS) {
    pinMode(pin, INPUT);
  }
  for (auto pin : OUTPUT_PINS) {
    pinMode(pin, OUTPUT);
  }
  turnScreenOn();
  turnAccelerometerOn();
  lcd.begin();
  lcd.setContrast(60);
  lcd.clear(); // Clear the screen
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("X: ");
  enableADC();
  lcd.println(getAccelerometerX());
  lcd.print("Y: ");
  lcd.println(getAccelerometerY());
  lcd.print("Z: ");
  lcd.println(getAccelerometerZ());
  disableADC();
  stayInDeepSleepFor(400, WDT_128ms);

  //  lcd.draw(platis_solutions_logo, sizeof(platis_solutions_logo) / sizeof(unsigned char));
  //  stayInDeepSleepFor(20000, WDT_8sec);
  //  lcd.draw(hi_im_dimitris, sizeof(hi_im_dimitris) / sizeof(unsigned char));
  //  stayInDeepSleepFor(30000, WDT_8sec);
  //  lcd.draw(aptiv_engineer, sizeof(aptiv_engineer) / sizeof(unsigned char));
  //  stayInDeepSleepFor(30000, WDT_8sec);
  //  lcd.draw(dimitris_picture, sizeof(dimitris_picture) / sizeof(unsigned char));
  //  stayInDeepSleepFor(15000, WDT_8sec);
}
