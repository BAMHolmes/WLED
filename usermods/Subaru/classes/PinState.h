#ifndef PIN_STATE_H
#define PIN_STATE_H

class PinState
{
public:
    bool current = false;
    unsigned int pinIndex; // Index of the pin in the expander
    unsigned int activationCount;
    // Constructor to set pin index during initialization
    PinState(unsigned int index) : pinIndex(index) {}

    bool hasChanged() const { return current != previous; }
    bool activated() const { return hasChanged() && !current; }
    bool deactivated() const { return hasChanged() && current; }
    bool isInputActive() const { return !current; }
    bool isInputInactive() const { return current; }
    bool isOutputActive() const { return current; }
    bool isOutputInactive() const { return !current; }

    void update(bool newState)
    {
        previous = current;
        current = newState;
    }

    // Method to write the state to the PCF8575
    void write(bool state);

private:
    bool previous = false;
};
#endif // PIN_STATE_H