#ifndef PIN_STATE_H
#define PIN_STATE_H
#include "Ticker.h"
#include "ColorPrint.h"
class PinState
{
private: 
    ColorPrint* p = ColorPrint::getInstance();
public:
    bool current = false;
    String name = "Unnamed Pin";
    PinState* dependency = nullptr;
    Ticker timer;  // Ticker object to manage timing

    unsigned int pinIndex; // Index of the pin in the expander
    unsigned int activationCount = 0;
    // Constructor to set pin index during initialization
    PinState(unsigned int index) : pinIndex(index) {}
    bool delayedDeactivationInProgress = false;
    bool hasChanged() const { return current != previous; }
    bool activated() const { return hasChanged() && !current; }
    bool deactivated() const { return hasChanged() && current; }
    bool isInputActive() const { return !current; }
    bool isInputInactive() const { return current; }
    bool isOutputActive() const { return current; }
    bool isOutputInactive() const { return !current; }
    void setName(String n) { name = n; };
    void addDependency(PinState* dep)
    {
        dependency = dep;
    }
    void update(bool newState)
    {
        previous = current;
        current = newState;
    }
    static void staticOff(PinState* p)
    {
        if (p)
        {
            p->write(false);
        }
    }

    void delayOff(unsigned long delay)
    {
        if (delayedDeactivationInProgress)
        {
            return;
        }
        delayedDeactivationInProgress = true;
        timer.once(delay, staticOff, this);
    }
    // Method to write the state to the PCF8575
    void write(bool state);

private:
    bool previous = false;
};
#endif // PIN_STATE_H