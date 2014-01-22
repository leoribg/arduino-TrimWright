
#include <Arduino.h>
#include <DFActors.h>
using namespace DFActors;


class Hardware {
    private:
        static const uint32_t TIMEOUT = 500; // milliseconds
        uint32_t timeout;
    public:
        void setup();
        void loop();
        void setLED(bool on);
        void sleep();
} hw;


enum {
    SIG_TIMER = SIG_USER
};


class Blink : public HSM {
    protected:
        DispatchOutcome stateROOT(const Event* event) {
            switch (event->signal) {
                case SIG_INIT:
                    return HSM_TRANSITION(&Blink::stateON);
                case SIG_SUPER:
                    return HSM_SUPER(NULL);
                default:
                    return HSM_HANDLED();
            }
        }

        DispatchOutcome stateON(const Event* event) {
            switch (event->signal) {
                case SIG_ENTER:
                    hw.setLED(true);
                    return HSM_HANDLED();
                case SIG_IDLE:
                    hw.sleep();
                    return HSM_HANDLED();
                case SIG_TIMER:
                    return HSM_TRANSITION(&Blink::stateOFF);
                default:
                    return HSM_SUPER(&Blink::stateROOT);
            }
        }

        DispatchOutcome stateOFF(const Event* event) {
            switch (event->signal) {
                case SIG_ENTER:
                    hw.setLED(false);
                    return HSM_HANDLED();
                case SIG_IDLE:
                    hw.sleep();
                    return HSM_HANDLED();
                case SIG_TIMER:
                    return HSM_TRANSITION(&Blink::stateON);
                default:
                    return HSM_SUPER(&Blink::stateROOT);
            }
        }
} blink;


void Hardware::setup() {
    pinMode(13, OUTPUT);
    timeout = millis() + TIMEOUT;
}


void Hardware::loop() {
    uint32_t now = millis();
    if (now >= timeout) {
        timeout = now + TIMEOUT;
        Event event;
        event.signal = (Signal) SIG_TIMER;
        blink.dispatch(&event);
    }
}


void Hardware::setLED(bool on) {
    digitalWrite(13, (on ? HIGH : LOW));
}


void Hardware::sleep() {
    // FUTURE:  put into a low power mode that still is woken by the timer
    // used by millis()
}


void setup() {
    hw.setup();
    blink.init();
}


void loop() {
    hw.loop();
}


