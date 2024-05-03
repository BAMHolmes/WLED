

#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>
#include "PinState.h"
#include "ColorPrint.h"
#include "Ticker.h"
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>

#ifndef SUBARU_TELEMETRY_H
#define SUBARU_TELEMETRY_H

#define PARK_PIN 0
#define RIGHT_SIGNAL_PIN 1
#define LEFT_SIGNAL_PIN 2
#define REVERSE_PIN 3
#define BRAKE_PIN 4
#define ROCKER_PASSENGER_LOW_PIN 6
#define ROCKER_PASSENGER_HIGH_PIN 7
#define PROJECTION_PIN 12
#define ENGINE_RELAY_PIN 13
#define INTERIOR_RELAY_PIN 14
#define UNDERCARRIAGE_RELAY_PIN 15
#define IGNITION_PIN 24
#define DOOR_PIN 25
#define LOCK_PIN 26
#define UNLOCK_PIN 27
#define PUSH_BUTTON_1_PIN 28
#define PUSH_BUTTON_2_PIN 29
#define ROCKER_DRIVER_HIGH_PIN 30
#define ROCKER_DRIVER_LOW_PIN 31

class SubaruTelemetry; // Forward declaration of SubaruTelemetry

class SubaruTelemetry
{
private:
    // std::vector<std::string> knownBLEAddresses = {"xx:xx:xx:xx:xx:xx", "yy:yy:yy:yy:yy:yy"}; // Replace with your device's BLE MAC addresses
    // int RSSI_THRESHOLD = -70; // Set your RSSI threshold for proximity
    // BLEScan* pBLEScan;

    // Plug A
    //---------------------------
    // P12 -> Projection Relay -> RJ45 0
    // P13 -> Engine Power Relay -> RJ45 1
    // P14-> Interior Power Relay -> RJ45 2
    // P07 <- Rocker High 2 <- RJ45 3
    // P06 <- Rocker Low 2 <- RJ45 4
    // IO14-> Data 5 -> RJ45 5
    // IO12 -> Data 3 -> RJ45 6
    // IO5-> Data 4 -> RJ45 7

    //---------------------------
    // Plug B
    //---------------------------
    // P0 / P16 <- Ignition <- RJ45 0
    // P1 / P17 <- Door <- RJ45 1
    // P2 / P18 <- Lock <- RJ45 2
    // P3 / P19 <- Unlock <- RJ45 3
    // P4 / P20 <- Push Button 1 <- RJ45 4
    // P5 / P21 <- Push Button 2 <- RJ45 5
    // P6 / P22 <- Rocker High 1 <- RJ45 6
    // P7 / P23 <- Rocker Low 1 <- RJ45 7

    // Plug C
    //---------------------------
    // IO18 -> Data 2 (Undercarriage) -> RJ45 7
    // IO15 -> Data 1 (Undercarriage) -> RJ45 6
    // P15 -> Undercarriage Relay -> RJ45 5
    // P04 <- Brake Signal <- RJ45 4
    // P03 <- Reverse Signal <- RJ45 3
    // P02 <- Left Signal <- RJ45 2
    // P01 <- Right Signal <- RJ45 1
    // P0 <- Park Signal <- RJ45 0
    Ticker relayOffTicker;  // Ticker object to manage timing
    bool relayShutdownInProgress = true;

    static SubaruTelemetry *instance;

public:
    uint32_t pinState = 0xFFFF0FFF; // Track the state of all 32 pins (input and output)
    // Declare all the variables and members that need to be initialized
    PinState parked{PARK_PIN};
    PinState right{RIGHT_SIGNAL_PIN};
    PinState left{LEFT_SIGNAL_PIN};
    PinState reverse{REVERSE_PIN};
    PinState brake{BRAKE_PIN};
    PinState passengerRockerHigh{ROCKER_PASSENGER_HIGH_PIN};
    PinState passengerRockerLow{ROCKER_PASSENGER_LOW_PIN};
    PinState projectionRelay{PROJECTION_PIN};
    PinState engineRelay{ENGINE_RELAY_PIN};
    PinState interiorRelay{INTERIOR_RELAY_PIN};
    PinState groundRelay{UNDERCARRIAGE_RELAY_PIN};
    PinState ignition{IGNITION_PIN};
    PinState doorOpen{DOOR_PIN};
    PinState doorLock{LOCK_PIN};
    PinState doorUnlock{UNLOCK_PIN};
    PinState buttonA{PUSH_BUTTON_1_PIN};
    PinState buttonB{PUSH_BUTTON_2_PIN};
    PinState driverRockerHigh{ROCKER_DRIVER_HIGH_PIN};
    PinState driverRockerLow{ROCKER_DRIVER_LOW_PIN};
    /**
     *  Constructor method SubaruTelemetry()
     */

    SubaruTelemetry()
    {
        groundRelay.addDependency(&buttonA);
        // readTelemetry();
        // pinMode(12, OUTPUT);
        // pinMode(14, OUTPUT);
        // Serial.begin(115200);
        // BLEDevice::init("");
        // pBLEScan = BLEDevice::getScan();
    }
    static SubaruTelemetry *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new SubaruTelemetry();
        }
        return instance;
    }
    static void writePin(unsigned int pin, bool state);
    static void writePCF8575(uint16_t dataFirst, uint16_t dataSecond)
    {
        Wire.beginTransmission(FIRST_PCF8575_ADDRESS);
        Wire.write(dataFirst & 0xFF);
        Wire.write(dataFirst >> 8);
        Wire.endTransmission();

        Wire.beginTransmission(SECOND_PCF8575_ADDRESS);
        Wire.write(dataSecond & 0xFF);
        Wire.write(dataSecond >> 8);
        Wire.endTransmission();

        // //Serial.println("Wrote to PCF8575: " + String(dataFirst) + " " + String(dataSecond));
        SubaruTelemetry::getInstance()->printState("Wrote to PCF8575");
    }

    void refreshInputPins()
    {
        // Output mask should only include the output pins P12 to P15 on the first PCF8575
        uint16_t maskOutputsFirst = 0xF000;  // P12 to P15 are outputs
        uint16_t maskOutputsSecond = 0x0000; // All pins on second PCF8575 are inputs, so no outputs to mask

        // For input pins, generally set them to high to enable input mode
        uint16_t defaultInputStateFirst = 0x0FFF; // Assume 1s for input pins P0 to P11 on the first PCF8575

        // Combine the current state of output pins with the default state for input pins
        // For the first expander, mask the outputs and set the rest as inputs
        uint16_t dataFirst = (pinState & maskOutputsFirst) | defaultInputStateFirst;

        // For the second expander, since all are inputs, write 0xFFFF
        uint16_t dataSecond = 0xFFFF; // Sets all pins to high (input mode)

        SubaruTelemetry::writePCF8575(dataFirst, dataSecond);
    }

    void initializePins()
    {
        Wire.begin(SDA_PIN, SCL_PIN);
        refreshInputPins(); // Set all pins to their correct initial state at setup
    }
    uint32_t readPCF8575()
    {
        uint32_t data = 0;
        uint16_t dataFirst = 0, dataSecond = 0;

        refreshInputPins();
        //  Read from the first PCF8575
        Wire.beginTransmission(FIRST_PCF8575_ADDRESS);
        Wire.endTransmission();
        Wire.requestFrom(FIRST_PCF8575_ADDRESS, 2);
        if (Wire.available() == 2)
        {
            dataFirst = Wire.read();
            dataFirst |= Wire.read() << 8;
        }

        // Read from the second PCF8575
        Wire.beginTransmission(SECOND_PCF8575_ADDRESS);
        Wire.endTransmission();
        Wire.requestFrom(SECOND_PCF8575_ADDRESS, 2);
        if (Wire.available() == 2)
        {
            dataSecond = Wire.read();       // Read the first byte
            dataSecond |= Wire.read() << 8; // Read the second byte and shift it by 8 bits
        }

        // Combine the data from both expanders
        uint32_t newData = ((uint32_t)dataSecond << 16) | dataFirst;

        // Define masks for preserving output pin states
        uint32_t maskOutputs = 0x0000F000;

        // Preserve output pin states and update input states
        pinState = (pinState & maskOutputs) | (newData & ~maskOutputs);

        updateHardware(); // Update the hardware with the new pin state
        printState("Read from PCF8575");
        return pinState;
    }

    void updateHardware()
    {
        // Log the full pin state before masking
        // for (int i = 31; i >= 0; i--)
        // {
        //     //Serial.print((pinState >> i) & 1);
        // }
        // //Serial.print(" <- Full pinState before mask");

        // //Serial.println();

        // Apply the correct mask
        uint16_t dataFirst = pinState & 0xFFFF;          // Assuming the first expander is connected to all lower 16 bits
        uint16_t dataSecond = (pinState >> 16) & 0xFFFF; // Assuming the second expander is connected to the upper 16 bits

        // Log what will be written to each PCF8575
        // for (int i = 15; i >= 0; i--)
        // {
        //     //Serial.print((dataFirst >> i) & 1);
        // }
        // //Serial.print(" <- Data to first PCF8575");

        // //Serial.println();

        // for (int i = 15; i >= 0; i--)
        // {
        //     //Serial.print((dataSecond >> i) & 1);
        // }
        // //Serial.print(" <- Data to second PCF8575");

        // //Serial.println();

        // Write to PCF8575
        SubaruTelemetry::writePCF8575(dataFirst, dataSecond);
    }
    // You would call refreshInputPins() periodically or after an event like this:
    // timer.every(1000, refreshInputPins); // Using a hypothetical timer library to call every 1000 ms
    void turnOnRelay(int segmentID);
    void turnOffRelay(int segmentID);
    void turnOffRelay(PinState &pinState, int delay);
    static void relayOffCallback(PinState *relay);  // Static callback method

    void turnOffAllRelays()
    {
        turnOffGroundLEDRelay();
        turnOffInteriorLEDRelay();
        turnOffProjectionRelay();
        turnOffEngineRelay();
    }
    void turnOffAllEffectRelays(){
        turnOffGroundLEDRelay();
        turnOffInteriorLEDRelay();
        turnOffEngineRelay();
    }
    void turnOffGroundLEDRelay()
    {
        //Serial.println("Turning off Ground Relay");
        groundRelay.write(false);
    }

    void turnOnGroundLEDRelay()
    {
        //Serial.println("Turning on Ground Relay");
        groundRelay.write(true);
    }

    void turnOffInteriorLEDRelay()
    {
        //Serial.println("Turning off Interior Relay");
        interiorRelay.write(false);
    }

    void turnOnInteriorLEDRelay()
    {
        //Serial.println("Turning on Interior Relay");
        interiorRelay.write(true);
    }

    void turnOffProjectionRelay()
    {
        //Serial.println("Turning off Projection Relay");
        projectionRelay.write(false);
    }

    void turnOnProjectionRelay()
    {
        //Serial.println("Turning on Projection Relay");
        projectionRelay.write(true);
    }

    void turnOffEngineRelay()
    {
        //Serial.println("Turning off Engine Relay");
        engineRelay.write(false);
    }

    void turnOnEngineRelay()
    {
        //Serial.println("Turning on Engine Relay");
        engineRelay.write(true);
    }
    bool alwaysTrue()
    {
        return true;
    }
    /**
     * A method that pulls PIN 9 high and retains the state of all other pins
     */
    void turnOnLEDRelay()
    {
        // //Serial.println("Turning on LED Relay on P15");
        // uint32_t currentState = readPCF8575();
        // currentState |= (1 << 15); // Set PIN 15 high
        // writePCF8575(currentState & 0xFFFF, currentState >> 16);
    }

    /**
     * A method that checks to see if PIN 9 is high, returns true if it is
     */
    bool LEDRelayOn()
    {
        return groundRelay.isOutputActive();
        // return relay; // Return true if pin 9 is high
    }

    /**
     * A method that checks to see if LEDRelayOn() is true, if it is return true, if not, turnOnLEDRelay then return true
     */
    void ensureRelayOn()
    {
        //Serial.println("Ensuring LED Relay is on");
        if (!LEDRelayOn())
        {
            //Serial.println("LED Relay is off, turning on...");
            turnOnGroundLEDRelay();
        }
        else
        {
            //Serial.println("LED Relay is on");
        }
    }
    static const int SDA_PIN = 21; // Make these constants static
    static const int SCL_PIN = 22;
    static const int FIRST_PCF8575_ADDRESS = 0x20;
    static const int SECOND_PCF8575_ADDRESS = 0x21;

    unsigned long lastTime = 0;
    unsigned long currentTime = 0;
    unsigned long period = 5000;
    void readTelemetry()
    {
        readPCF8575();
        // Reset input pins every 1000 milliseconds
        // auto currentTime = millis();
        // if (currentTime - lastTime > period)
        // {
        //     refreshInputPins();
        //     lastTime = currentTime;
        // }
        // Read the state of all 32 pins from the expanders
        // Loop through the state of all combined 32 pins of both PCF857 expanders and print each pin state
        /**
        for (int i = 0; i < 32; i++)
        {
            const String stateString = pinState & (1 << i) ? "HIGH" : "LOW";
            //Serial.print("P" + String(i) + ":" + stateString);
        }
        //Serial.println();
        */
        // Update each pin state based on the corresponding bit in the pinState variable
        // //Serial.println("Reading telemetry and updating PinState objects...");
        parked.update(pinState & (1 << PARK_PIN));
        right.update(pinState & (1 << RIGHT_SIGNAL_PIN));
        left.update(pinState & (1 << LEFT_SIGNAL_PIN));
        reverse.update(pinState & (1 << REVERSE_PIN));
        brake.update(pinState & (1 << BRAKE_PIN));
        passengerRockerLow.update(pinState & (1 << ROCKER_PASSENGER_LOW_PIN));
        passengerRockerHigh.update(pinState & (1 << ROCKER_PASSENGER_HIGH_PIN));
        // projectionRelay.update(pinState & (1 << PROJECTION_PIN));
        // engineRelay.update(pinState & (1 << ENGINE_RELAY_PIN));
        // interiorRelay.update(pinState & (1 << INTERIOR_RELAY_PIN));
        // groundRelay.update(pinState & (1 << UNDERCARRIAGE_RELAY_PIN));

        ignition.update(pinState & (1 << IGNITION_PIN));
        doorOpen.update(pinState & (1 << DOOR_PIN));
        doorLock.update(pinState & (1 << LOCK_PIN));
        doorUnlock.update(pinState & (1 << UNLOCK_PIN));
        buttonA.update(pinState & (1 << PUSH_BUTTON_1_PIN));
        buttonB.update(pinState & (1 << PUSH_BUTTON_2_PIN));
        driverRockerHigh.update(pinState & (1 << ROCKER_DRIVER_HIGH_PIN));
        driverRockerLow.update(pinState & (1 << ROCKER_DRIVER_LOW_PIN));

        // Print the current state of all relays every 1000 milliseconds
        auto currentTime = millis();
        if (currentTime - lastTime > period)
        {
            printAllRelays();
            lastTime = currentTime;
        }
    }
    // Print the state of all pins. Takes "state" as an optional parameter
    void printState(String postfix = "Verified State")
    {
        return;
        // If the default value is passed, read the current state
        auto currentTime = millis();
        if (currentTime - lastTime > period)
        {
            for (int i = 31; i >= 0; i--)
            {
                //Serial.print((pinState >> i) & 1);
            }
            //Serial.print(" <- " + postfix);
            //Serial.println(); // Print newline after the binary data
            lastTime = currentTime;
        }
    }
    void printAllRelays()
    {
        //Serial.println("projectionRelay: " + String(projectionRelay.current));
        //Serial.println("engineRelay: " + String(engineRelay.current));
        //Serial.println("interiorRelay: " + String(interiorRelay.current));
        //Serial.println("groundRelay: " + String(groundRelay.current));
        //Serial.println();
    }
};

// Now define methods that depend on the complete definition of SubaruTelemetry
void PinState::write(bool state)
{
    //String dependencyDefined = dependency ? " dependency is defined. :) " : " dependency is NOT defined!!!";
    //p.println("Pin " + String(pinIndex) + dependencyDefined, ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
    //Check if this PinState has a dependency, if it doesn check that the dependency is active.
    if (dependency && !dependency->isInputActive())
    {
        //p.println("Dependency not active, cannot writing to pin " + String(pinIndex) + " with state " + String(state), ColorPrint::FG_WHITE, ColorPrint::BG_RED);
        return;
    }
    //Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    if (state && !current)
    {
        current = true;
        SubaruTelemetry::writePin(pinIndex, current); // Pass the correct pin index and state
        //Serial.println("Writing to pin _" + String(pinIndex) + "_ via PinState::write, State: " + String(current));
    }
    else if (!state && current)
    {
        current = false;
        SubaruTelemetry::writePin(pinIndex, current); // Pass the correct pin index and state
        //Serial.println("Writing to pin _" + String(pinIndex) + "_ via PinState::write, State: " + String(current));
    }
    //Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

}

void SubaruTelemetry::writePin(unsigned int pin, bool state)
{
    SubaruTelemetry *instance = SubaruTelemetry::getInstance(); // Get the instance
    uint32_t *pinStatePtr = &(instance->pinState);              // Get pointer to pinState

    //Serial.print("Attempting to ");
    //Serial.print(state ? "set" : "clear");
    //Serial.print(" pin ");
    //Serial.print(pin);
    //Serial.print(", pinState before: ");
    for (int i = 31; i >= 0; i--)
    {
        //Serial.print((*pinStatePtr >> i) & 1); // Dereference pointer for reading
    }
    //Serial.println();

    uint32_t mask = 1UL << pin;
    if (state)
    {
        *pinStatePtr |= mask; // Use dereferenced pointer to set the pin bit
    }
    else
    {
        *pinStatePtr &= ~mask; // Use dereferenced pointer to clear the pin bit
    }

    //Serial.print("pinState after: ");
    for (int i = 31; i >= 0; i--)
    {
        //Serial.print((*pinStatePtr >> i) & 1); // Dereference pointer for reading
    }
    //Serial.println();
    SubaruTelemetry::getInstance()->updateHardware();
}

// Definitions of other SubaruTelemetry methods...

SubaruTelemetry *SubaruTelemetry::instance = nullptr; // Initialize the static instance variable
SubaruTelemetry *ST = SubaruTelemetry::getInstance();

#endif // SUBARU_TELEMETRY_H
