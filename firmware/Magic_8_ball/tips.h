#include <avr/pgmspace.h>

const uint8_t AMOUNT_OF_TIPS = 24;

// Manually insert newlines for the text to wrap around the edges.
// Each line can fit 17 characters so place a newline character
// at some point before that so to break the word nicely.
const char tip_0[] PROGMEM = "Have you talked\nto Jesper about\nthis?";
const char tip_1[] PROGMEM = "Have you talked\nto David about\nthis?";
const char tip_2[] PROGMEM = "Have you tried\na clean build?";
const char tip_3[] PROGMEM = "Maybe you can\nturn it on and\noff?";
const char tip_4[] PROGMEM = "I am not sure\ntry again!";
const char tip_5[] PROGMEM = "Have you tried\nto unit test\nit?";
const char tip_6[] PROGMEM = "Could there be\nsome race\ncondition?";
const char tip_7[] PROGMEM = "Try again\nafter having\na coffee!";
const char tip_8[] PROGMEM = "Walk around\nthe office and\ntry again!";
const char tip_9[] PROGMEM = "Have you tried\npair programming\nthis?";
const char tip_10[] PROGMEM = "Debug it with\nprintouts!";
const char tip_11[] PROGMEM = "Is this actually\na problem?";
const char tip_12[] PROGMEM = "Can you give me\nmore details and\nask again?";
const char tip_13[] PROGMEM = "Can the code be\nsimplified?";
const char tip_14[] PROGMEM = "Maybe some of\nthe threads are\nunnecessary?";
const char tip_15[] PROGMEM = "Can the problem\nbe broken down\nmore?";
const char tip_16[] PROGMEM = "Call for a group\nmeeting to\ndiscuss this!";
const char tip_17[] PROGMEM = "Should you\nreally be doing\nthis?";
const char tip_18[] PROGMEM = "Maybe you should\nconsider some\nrefactoring!";
const char tip_19[] PROGMEM = "Ask the person\nnext to you for\nhelp!";
const char tip_20[] PROGMEM = "Can this be\nhardware\nrelated?";
const char tip_21[] PROGMEM = "Have you tried\ngoogling it?";
const char tip_22[] PROGMEM = "Could this not\nbe your fault?";
const char tip_23[] PROGMEM = "Have you tried\nstep by step\nexecution?";

const char* const TIPS[AMOUNT_OF_TIPS] PROGMEM = {
    tip_0,
    tip_1,
    tip_2,
    tip_3,
    tip_4,
    tip_5,
    tip_6,
    tip_7,
    tip_8,
    tip_9,
    tip_10,
    tip_11,
    tip_12,
    tip_13,
    tip_14,
    tip_15,
    tip_16,
    tip_17,
    tip_18,
    tip_19,
    tip_20,
    tip_21,
    tip_22,
    tip_23
};
