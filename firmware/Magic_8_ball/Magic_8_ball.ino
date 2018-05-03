#include <Nokia_LCD.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "bitmaps.h"

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
  // Ball facing up
  // Screen is on - Waiting display
  // Fast check of accelerometer for changes
  IDLE_SCREEN,
  // Ball facing up
  // Screen is on - Game views
  // Faster check of accelerometer for changes
  PLAYING,
};

const uint8_t V_ACC_PIN = 8; // PB2
const uint8_t CLK_PIN = 9; // PB1
const uint8_t V_DIS_PIN = 10; // PB0
const uint8_t X_OUT_PIN = A7; // PA7
const uint8_t Y_OUT_PIN = A6; // PA6
const uint8_t Z_OUT_PIN = A5; // PA5
const uint8_t RST_PIN = 4; // PA4
const uint8_t DIN_PIN = 3; // PA3
const uint8_t CE_PIN = 2; // PA2
const uint8_t DC_PIN = 1; // PA1
const uint8_t LED_PIN = 0; // PA0

// Input & output pins directly controlled in this scope
const uint8_t OUTPUT_PINS[] = {V_ACC_PIN, V_DIS_PIN, LED_PIN};
const uint8_t INPUT_PINS[] = {X_OUT_PIN, Y_OUT_PIN, Z_OUT_PIN};

// The experimentally determined Z-axis value over which we consider
// ourselves to be facing up
const unsigned int MIN_FACING_UP_THRESHOLD = 520;
const uint8_t TIMES_TO_CHECK_FOR_MOVEMENT = 20;
// The minimum acceleration delta (on X,Y,Z axes) to be considered as a movement
// The smaller the number the easier a "movement" will be registered
const unsigned int MOVEMENT_ACCELERATION_THRESHOLD = 30; // Experimentally determined
// The amount of detected movements needed to start a game
const uint8_t AMOUNT_OF_MOVEMENTS_THRESHOLD = 5;
// How much to wait before using the accelerometer in milliseconds
const unsigned long ACCELEROMETER_BOOTUP_TIME = 15;
// Sleep duration while in DEEP_SLEEP mode
const unsigned long DEEP_SLEEP_INTERVAL = 2000;
// Sleep duration while in IDLE_SCREEN mode
const unsigned long IDLE_SCREEN_INTERVAL = 500;
// Sleep duration while in PLAYING mode
const unsigned long PLAYING_INTERVAL = 15000;

volatile bool watchdogBarked = false;

Nokia_LCD lcd(CLK_PIN /* CLK */, DIN_PIN /* DIN */, DC_PIN /* DC */, CE_PIN /* CE */, RST_PIN /* RST */);
RunningState currentState = DEEP_SLEEP;

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
  wdt_reset(); // Pat the dog
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
  // Accelerometer needs some few milliseconds to get ready
  // so we need to make sure that we give it this time
  digitalWrite(V_ACC_PIN, HIGH);
  stayInDeepSleepFor(ACCELEROMETER_BOOTUP_TIME, WDT_16ms); // Give the accelerometer some time to boot
  enableADC();
}

void turnAccelerometerOff() {
  disableADC();
  digitalWrite(V_ACC_PIN, LOW);
}

unsigned int getAccelerationX() {
  return analogRead(X_OUT_PIN);
}

unsigned int getAccelerationY() {
  return analogRead(Y_OUT_PIN);
}

unsigned int getAccelerationZ() {
  return analogRead(Z_OUT_PIN);
}

unsigned int getAccelerationXYZ() {
  return getAccelerationX() + getAccelerationY() + getAccelerationZ();
}

void disableADC() {
  ADCSRA &= ~(1 << ADEN);
}

void enableADC() {
  ADCSRA |= (1 << ADEN);
}

bool isFacingUp() {
  return getAccelerationZ() >= MIN_FACING_UP_THRESHOLD;
}

/*** Helper functions ***/

void setup() {
  // Set pin directions
  for (auto pin : INPUT_PINS) {
    pinMode(pin, INPUT);
  }
  for (auto pin : OUTPUT_PINS) {
    pinMode(pin, OUTPUT);
  }

  // Disable digital pin buffers for analog pins
  // to save some power, since we are doing only analog readings
  bitSet(DIDR0, ADC5D); // Disable digital buffer on A5
  bitSet(DIDR0, ADC6D); // Disable digital buffer on A6
  bitSet(DIDR0, ADC7D); // Disable digital buffer on A7

  turnBacklightOff();
}

void loop() {
  switch (currentState) {
    case DEEP_SLEEP:
      {
        turnAccelerometerOn();
        // Store whether it is facing up in a variable
        // so we can turn the accelerometer off right away
        // before printing to the screen or sleeping more
        bool facingUp = isFacingUp();
        turnAccelerometerOff();
        if (facingUp) {
          currentState = IDLE_SCREEN;
          turnScreenOn();
          lcd.begin();
          lcd.setContrast(60);
          lcd.clear(); // Clear the screen
          lcd.println("Ask your question and shake to get your answer");
        } else {
          turnScreenOff();
          stayInDeepSleepFor(DEEP_SLEEP_INTERVAL, WDT_1sec);
        }
      }
      break;
    case IDLE_SCREEN:
      {
        bool hasMovedEnough = false;
        unsigned int movements = 0;
        turnAccelerometerOn();
        int previousAcceleration = getAccelerationXYZ(); // Some initial measurement
        turnAccelerometerOff();

        // Check for movements
        for (uint8_t i = 0; i < TIMES_TO_CHECK_FOR_MOVEMENT && !hasMovedEnough; i++) {
          stayInDeepSleepFor(IDLE_SCREEN_INTERVAL, WDT_64ms);

          turnAccelerometerOn();
          int currentAcceleration = getAccelerationXYZ();
          turnAccelerometerOff();
          int accelerationDelta = currentAcceleration - previousAcceleration;
          previousAcceleration = currentAcceleration;

          // Determine whether there was a movement
          if (abs(accelerationDelta) >= MOVEMENT_ACCELERATION_THRESHOLD) {
            if (++movements >= AMOUNT_OF_MOVEMENTS_THRESHOLD) {
              hasMovedEnough = true;
            }
            // Print out visual feedback/affirmation for the user's movement
            lcd.print("*");
          }
        }

        // Determine the state transition
        currentState = hasMovedEnough ? PLAYING : DEEP_SLEEP;
      }
      break;
    case PLAYING:
      {
        lcd.clear();
        // Display a very helpful message to the user
        lcd.println("A helpful tip to the user");
        stayInDeepSleepFor(PLAYING_INTERVAL, WDT_8sec);

        currentState = IDLE_SCREEN;
      }
      break;
    default:
      break;
  }

  //  lcd.draw(platis_solutions_logo, sizeof(platis_solutions_logo) / sizeof(unsigned char));
  //  stayInDeepSleepFor(20000, WDT_8sec);
  //  lcd.draw(hi_im_dimitris, sizeof(hi_im_dimitris) / sizeof(unsigned char));
  //  stayInDeepSleepFor(30000, WDT_8sec);
  //  lcd.draw(aptiv_engineer, sizeof(aptiv_engineer) / sizeof(unsigned char));
  //  stayInDeepSleepFor(30000, WDT_8sec);
  //  lcd.draw(dimitris_picture, sizeof(dimitris_picture) / sizeof(unsigned char));
  //  stayInDeepSleepFor(15000, WDT_8sec);
}