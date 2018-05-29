# Dialectic Ball
Need to debug your code? Got stuck? Stack Overflow doesn't have the answer? Then ask Dialectic Ball!

## What?
The Dialectic Ball is a physical, [Magic 8-ball](https://en.wikipedia.org/wiki/Magic_8-Ball) inspired, debugging tool for your daily code struggles! Once you are out of ideas, pick up the Dialectic Ball and explain the problem you are facing with all its details. Remember, the Dialectic Ball is a delicate and highly advanced piece of technology, thus the more information you provide it and the more you outline the problem with all its aspects, the better the chance it will be able to assist you.

Once you are done illustrating your coding issue to it, shake Dialectic Ball until the progress bar is filled and wait for the answer! If you do not get a satisfactory answer then try again. Clarify the problem better, think of any crucial details that you might have omitted during the previous run and shake again. Keep on it, until you get a witty response that helps you solve the problem!

The Dialectic Ball is more like a [rubber duck](https://en.wikipedia.org/wiki/Rubber_duck_debugging) than a [Magic 8-ball](https://en.wikipedia.org/wiki/Magic_8-Ball), so **do not ask it yes-no questions**. That wouldn't be so dialectic, would it? Of course, you can always easily modify the code and make it answer whatever you wish.

### Usage manual
1. Pick up the Dialectic Ball and face the screen
2. Admire the 8-ball intro logo
3. Dialectic Ball will prompt you to ask your question
4. Take your time and ask your question, providing all the necessary details
5. When you are done, shake the ball until the progress bar on the bottom is filled up
6. The solution to your problems is presented to you
7. If you are satisfied with the response, put the Dialectic Ball facing downwards
8. If the answer did not help, shake again when prompted

## Why?
Despite our impressive rubber duck collection at the office, sometimes they fall short on feedback and interaction. In such occasions we often utilize a colleague, but this is admittedly not the most productive use of our colleagues. Dialectic Ball intends to fill this gap, i.e. a rubber duck that is able to provide feedback. As a bonus, shaking it is fun!

## How?
Dialectic Ball is comprised of an ATtiny84, connected to a GY-61 module (based on the ADXL335 accelerometer) and an LCD screen like the ones used on the old NOKIA phones! Two AAA batteries are powering the gadget up and a [3D printed case](https://www.tinkercad.com/things/4zkr0X7OHBL) encompasses everything, giving it a hemispherical shape. The PCB that routes everything together was manufactured by [JLCPCB](https://jlcpcb.com/), who will make your prototype PCB's for just 2$ and were kind enough to sponsor the `rev. 0` boards.

From a software perspective the microcontroller mainly remains in deep sleep mode, periodically waking up to turn the accelerometer on and read its measurements. If necessary, the screen will also be powered up and the appropriate messages will be displayed to the user. Specifically, the system wakes up when the ball is not facing downwards. Furthermore, the particular LCD screen consumes minimal power, even when displaying graphics, which makes it ideal for low power applications. Overall, the average power consumption of the gadget is around 10μA, enabling it to remain operational for years on those two batteries.

### Components
* [Dialectic Ball PCB](https://github.com/platisd/dialectic-ball/tree/master/hardware/eagle)
* ATTiny84-20PU
* DIP-14 socket
* AAA battery holders (2)
* Nokia LCD screen (**Important**: Get ones that have [this](https://i.imgur.com/MhUsdiz.jpg) pin layout)
* GY-61 accelerometer module
* Tactile button
* 10KΩ resistor
* 330Ω resistor

## Media
* [Article on platis.solutions](https://platis.solutions/blog/2018/05/30/debug-your-code-with-dialectic-ball)
* [Demo video](https://youtu.be/oXdIgaYoSw0)

## Similar projects
* [Programmer's Magic 8-ball](https://github.com/FareedQ/Programmer-s-Magic-8-Ball)
