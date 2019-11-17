#include <timer.h>

// create a timers with default settings
auto timer1 = timer_create_default();
auto timer2 = timer_create_default();


// print number of milliseconds since processor was booted
bool print_message1(void *) {
    Serial.print("timer1 called at: ");
    Serial.println(millis());

    return true; // repeat? true
}

// print number of milliseconds since processor was booted
bool print_message2(void *) {
    Serial.print("*timer2 called at: ");
    Serial.println(millis());

    return true; // repeat? true
}

void setup() {
    Serial.begin(9600);

    // set the callback print_message function for execution every 1000 millis (1 second)
    timer1.every(1000, print_message1);
    timer2.every(10000, print_message2);
}

void loop() {
    timer1.tick(); // tick the timer
    timer2.tick(); // tick the timer
}

