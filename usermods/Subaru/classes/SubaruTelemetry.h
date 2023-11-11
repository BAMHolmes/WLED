

#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>

#ifndef SUBARU_TELEMETRY_H
#define SUBARU_TELEMETRY_H
class SubaruTelemetry
{
private:
    bool left_indicator_on = false;
    bool right_indicator_on = false;
    bool brake_pedal_pressed = false;
    bool door_is_open = false;
    bool doors_unlocked = false;
    bool doors_locked = false;
    bool car_in_reverse = false;
    bool ignition = true;

    bool brake_pedal_previous_state = false;
    bool reverse_previous_state = false;
    bool left_indicator_previous_state = false;
    bool right_indicator_previous_state = false;
    bool door_previous_state = false;
    bool doors_unlocked_previous_state = false;
    bool doors_locked_previous_state = false;
    bool ignition_previous_state = false;

    bool brake_pedal_changed = false;
    bool reverse_changed = false;
    bool left_indicator_changed = false;
    bool right_indicator_changed = false;
    bool door_changed = false;
    bool doors_unlocked_changed = false;
    bool doors_locked_changed = false;
    bool ignition_changed = false;
    

    uint16_t readPCF8575()
    {
        uint16_t data = 0;
        Wire.beginTransmission(PCF8575_ADDRESS); // Begin transmission to PCF8575
        Wire.endTransmission();                  // End transmission

        Wire.requestFrom(PCF8575_ADDRESS, 2); // Request 2 bytes from PCF8575
        if (Wire.available())
        {
            data = Wire.read();       // Read the low byte
            data |= Wire.read() << 8; // Read the high byte and shift it left
        }
        return data;
    }


public:
    // Declare all the variables and members that need to be initialized

    /**
     *  Constructor method SubaruTelemetry()
     */

    SubaruTelemetry()
    {
        readTelemetry();
    }

    static const int SDA_PIN = 21; // Make these constants static
    static const int SCL_PIN = 22;
    static const int PCF8575_ADDRESS = 0x20;

    bool brake_pedal_change_to_on = false;
    bool reverse_change_to_on = false;
    bool left_indicator_change_to_on = false;
    bool right_indicator_change_to_on = false;
    bool door_change_to_on = false;
    bool doors_unlocked_change_to_on = false;
    bool doors_locked_change_to_on = false;
    bool ignition_change_to_on = false;


    bool EXT_PIN_1 = false, EXT_PIN_2 = false, EXT_PIN_3 = false, EXT_PIN_4 = false, EXT_PIN_5 = false, EXT_PIN_6 = false, EXT_PIN_7 = false, EXT_PIN_8 = false,
         EXT_PIN_9 = false, EXT_PIN_10 = false, EXT_PIN_11 = false, EXT_PIN_12 = false, EXT_PIN_13 = false, EXT_PIN_14 = false, EXT_PIN_15 = false, EXT_PIN_16 = false;
    unsigned long lastTime = 0;
    unsigned long currentTime = 0;
    unsigned long period = 10000;


  
    void readTelemetry()
    {

        uint16_t pinState = readPCF8575(); // Read the state of all pins

        // Assign the state of each pin
        EXT_PIN_1 = (pinState & 0x0001) != 0;
        EXT_PIN_2 = (pinState & 0x0002) != 0;
        EXT_PIN_3 = (pinState & 0x0004) != 0;
        EXT_PIN_4 = (pinState & 0x0008) != 0;
        EXT_PIN_5 = (pinState & 0x0010) != 0;
        EXT_PIN_6 = (pinState & 0x0020) != 0;
        EXT_PIN_7 = (pinState & 0x0040) != 0;
        EXT_PIN_8 = (pinState & 0x0080) != 0;
        EXT_PIN_9 = (pinState & 0x0100) != 0;
        EXT_PIN_10 = (pinState & 0x0200) != 0;
        EXT_PIN_11 = (pinState & 0x0400) != 0;
        EXT_PIN_12 = (pinState & 0x0800) != 0;
        EXT_PIN_13 = (pinState & 0x1000) != 0;
        EXT_PIN_14 = (pinState & 0x2000) != 0;
        EXT_PIN_15 = (pinState & 0x4000) != 0;
        EXT_PIN_16 = (pinState & 0x8000) != 0;

        right_indicator_on = EXT_PIN_2;
        left_indicator_on = EXT_PIN_3;
        brake_pedal_pressed = EXT_PIN_4;
        door_is_open = EXT_PIN_1;
        car_in_reverse = EXT_PIN_5;
        doors_locked = EXT_PIN_6;
        doors_unlocked = EXT_PIN_7;
        ignition = EXT_PIN_8;

        //Set all the "changed" variables depending on the previous state
        brake_pedal_changed = brake_pedal_pressed != brake_pedal_previous_state;
        reverse_changed = car_in_reverse != reverse_previous_state;
        left_indicator_changed = left_indicator_on != left_indicator_previous_state;
        right_indicator_changed = right_indicator_on != right_indicator_previous_state;
        door_changed = door_is_open != door_previous_state;
        doors_unlocked_changed = doors_unlocked != doors_unlocked_previous_state;
        doors_locked_changed = doors_locked != doors_locked_previous_state;
        ignition_changed = ignition != ignition_previous_state;

        //Set all the "change to on" variables depending on the previous state and the changed state
        brake_pedal_change_to_on = brake_pedal_changed && brake_pedal_pressed;
        reverse_change_to_on = reverse_changed && car_in_reverse;
        left_indicator_change_to_on = left_indicator_changed && left_indicator_on;
        right_indicator_change_to_on = right_indicator_changed && right_indicator_on;
        door_change_to_on = door_changed && door_is_open;
        doors_unlocked_change_to_on = doors_unlocked_changed && doors_unlocked;
        doors_locked_change_to_on = doors_locked_changed && doors_locked;
        ignition_change_to_on = ignition_changed && ignition;

        //Set all the previous state variables
        brake_pedal_previous_state = brake_pedal_pressed;
        reverse_previous_state = car_in_reverse;
        left_indicator_previous_state = left_indicator_on;
        right_indicator_previous_state = right_indicator_on;
        door_previous_state = door_is_open;
        doors_unlocked_previous_state = doors_unlocked;
        doors_locked_previous_state = doors_locked;
        ignition_previous_state = ignition;
    }
    bool brakeEngaged()
    {
        return brake_pedal_change_to_on;
    }
    bool brakeStatus(){
        return brake_pedal_pressed;
    }
    bool leftIndicatorOn()
    {
        return left_indicator_change_to_on;
    }
    bool leftIndicatorStatus(){
        return left_indicator_on;
    }
    bool rightIndicatorOn()
    {
        return right_indicator_change_to_on;
    }
    bool rightIndicatorStatus(){
        return right_indicator_on;
    }
    bool doorOpen()
    {
        return door_change_to_on;
    }
    bool doorStatus(){
        return door_is_open;
    }
    bool unlocked()
    {
        return doors_unlocked_change_to_on;
    }
    bool unlockedStatus(){
        return doors_unlocked;
    }
    bool locked()
    {
        return doors_locked_change_to_on;
    }
    bool lockedStatus(){
        return doors_locked;
    }
    bool reverseEngaged()
    {
        return reverse_change_to_on;
    }
    bool reverseStatus(){
        return car_in_reverse;
    }
    bool ignitionOn()
    {
        return ignition_change_to_on;
    }
    bool ignitionStatus(){
        return ignition;
    }
    bool trueStatus(){
        return true;
    }

};
SubaruTelemetry ST;

#endif // SUBARU_TELEMETRY_H
