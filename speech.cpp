#include "speech.h"

#include "globals.h"
#include "hardware.h"

void draw_game(int init);

/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

#define YELLOW 0xFFFF00

void draw_speech_bubble()
{
    uLCD.filled_rectangle(0, 91, 127, 117, BLACK);
    uLCD.rectangle(0, 91, 127, 117, YELLOW);
}

void erase_speech_bubble()
{
    draw_game(1);
}

void draw_speech_line(const char* line, int which)
{
    uLCD.locate(1, 12+which);
    uLCD.printf("%s", line);
}

void speech_bubble_wait()
{
    wait_ms(500);
    GameInputs in = read_inputs();
    while (in.b1 == 1) {
        in = read_inputs();
    }
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}

void long_speech(const char* lines[], int n)
{
    // should be a for loop process to index into array and print lines 1 and 2
}
