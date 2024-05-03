#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

#include <Arduino.h>
#include <vector>

/**
 * A Class with methods for printing colored text to Serial monitor.
 * It accepts a String and a color code for the foreground and background color of the text.
 * The color codes are defined in the Color enum.
 * The print and println methods buffer the output until commit is called to send it to the Serial.
 */

class ColorPrint
{
public:
    enum Color
    {
        FG_BLACK = 30,
        FG_RED = 31,
        FG_GREEN = 32,
        FG_YELLOW = 33,
        FG_BLUE = 34,
        FG_MAGENTA = 35,
        FG_CYAN = 36,
        FG_WHITE = 37,
        BG_BLACK = 40,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_YELLOW = 43,
        BG_BLUE = 44,
        BG_MAGENTA = 45,
        BG_CYAN = 46,
        BG_WHITE = 47,
        BG_GRAY = 100
    };

private:
    static ColorPrint* instance;
    std::vector<String> buffer;
    bool enabled = true;

    ColorPrint() {}  // Private constructor for singleton

public:
    // Get the singleton instance of ColorPrint
    void disable(){
        enabled = false;
    }
    void enable(){
        enabled = true;
    }
    static ColorPrint* getInstance() {
        if (instance == nullptr) {
            instance = new ColorPrint();
        }
        return instance;
    }

    // Disable or enable color print messages
    void setEnabled(bool enable) {
        enabled = enable;
    }

    void print(const String& text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK) {
        if (!enabled) return;
        Serial.print("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
        //buffer.push_back("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");

    }

    void println(const String& text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK) {
        if (!enabled) return;
        Serial.println("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
        //buffer.push_back("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m\n");
    }


};

// Initialize the static instance pointer
ColorPrint* ColorPrint::instance = nullptr;

#endif // COLOR_PRINT_H
