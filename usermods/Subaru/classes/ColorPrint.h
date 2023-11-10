
#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

/**
 * A Class with methods for printing colored text to Serial monitor.
 * It accepts a String and a color code for the foreground and background color of the text .
 * The color codes should be enums like FG_RED, BG_GREEN for forground red and background green respectfully
 */

class ColorPrint
{
public:
    static bool enabled;
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

    static void print(String text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK)
    {
        if(!enabled) return;
        Serial.print("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
    }
    static void println(String text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK)
    {
        if(!enabled) return;
        Serial.println("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
    }
};
// Disable or enable color print messages
bool ColorPrint::enabled = true;


const ColorPrint p;
#endif // COLOR_PRINT_H
