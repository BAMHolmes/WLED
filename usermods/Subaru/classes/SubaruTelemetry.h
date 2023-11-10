

#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>

#ifndef SUBARU_TELEMETRY_H
#define SUBARU_TELEMETRY_H

#define REAR_SEGMENT 0
#define LEFT_SEGMENT 1
#define FRONT_SEGMENT 2
#define RIGHT_SEGMENT 3
#define UNIFIED_SEGMENT 4

struct SubaruSegmentConfig {
    int frontStart;
    int frontEnd;
    int leftStart;
    int leftEnd;
    int rightStart;
    int rightEnd;
    int rearStart;
    int rearEnd;
    int unifiedStart;
    int unifiedEnd;
};

#define RIGHT_SEGMENT_START 0
#define RIGHT_SEGMENT_END 200
#define FRONT_SEGMENT_START 200
#define FRONT_SEGMENT_END 240
#define LEFT_SEGMENT_START 240
#define LEFT_SEGMENT_END 440
#define REAR_SEGMENT_START 440
#define REAR_SEGMENT_END 480
#define UNIFIED_SEGMENT_START 0
#define UNIFIED_SEGMENT_END 480

#define INSTANT_TRANSITION 0
#define MEDIUM_TRANSITION 1000
#define SLOW_TRANSITION 2000

static SubaruSegmentConfig SUBARU_SEGMENT_CONFIG = {FRONT_SEGMENT_START, FRONT_SEGMENT_END, LEFT_SEGMENT_START, LEFT_SEGMENT_END, RIGHT_SEGMENT_START, RIGHT_SEGMENT_END, REAR_SEGMENT_START, REAR_SEGMENT_END, UNIFIED_SEGMENT_START, UNIFIED_SEGMENT_END};
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
    
  void selectOnly(int segment)
    {
        rightSegment().setOption(SEG_OPTION_SELECTED, 0);
        leftSegment().setOption(SEG_OPTION_SELECTED, 0);
        rearSegment().setOption(SEG_OPTION_SELECTED, 0);
        frontSegment().setOption(SEG_OPTION_SELECTED, 0);
        seg(segment).setOption(SEG_OPTION_SELECTED, 1);
    }
    void enableOnly(int segment){
        rightSegment().setOption(SEG_OPTION_ON, 0);
        leftSegment().setOption(SEG_OPTION_ON, 0);
        rearSegment().setOption(SEG_OPTION_ON, 0);
        frontSegment().setOption(SEG_OPTION_ON, 0);
        seg(segment).setOption(SEG_OPTION_ON, 1);
    }
    void disable(int segment){
        seg(segment).setOption(SEG_OPTION_ON, 0);
    }
    void selectAndEnableOnly(int segment){
        selectOnly(segment);
        enableOnly(segment);
    }
    void selectAll()
    {
        rightSegment().setOption(SEG_OPTION_SELECTED, 1);
        leftSegment().setOption(SEG_OPTION_SELECTED, 1);
        rearSegment().setOption(SEG_OPTION_SELECTED, 1);
        frontSegment().setOption(SEG_OPTION_SELECTED, 1);
    }
    void enableAll(){
        rightSegment().setOption(SEG_OPTION_ON, 1);
        leftSegment().setOption(SEG_OPTION_ON, 1);
        rearSegment().setOption(SEG_OPTION_ON, 1);
        frontSegment().setOption(SEG_OPTION_ON, 1);
    }
    void selectAndEnableAll(){
        selectAll();
        enableAll();
    }

    void select(int segment){
        seg(segment).setOption(SEG_OPTION_SELECTED, 1);
    }

    void enable(int segment){
        seg(segment).setOption(SEG_OPTION_ON, 1);
    }

    void selectAndEnable(int segment){
        select(segment);
        enable(segment);
    }
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
        setupSegments();
    }

    static const int SDA_PIN = 21; // Make these constants static
    static const int SCL_PIN = 22;
    static const int PCF8575_ADDRESS = 0x20;

    bool paused = false;
    bool flashingRed = false;
    bool welcomeLights = false;


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

    uint8_t previousBrightness = 255;
    uint8_t previousEffect = 0;
    uint8_t previousPalette = 0;

    uint8_t preBrakeBrightness = 255;
    uint8_t preDoorBrightness = 255;
    uint8_t preRightBrightness = 255;
    uint8_t preLeftBrightness = 255;
    uint8_t preFrontBrightness = 255;

    // Declare all the Segment members
    Segment brake_segment_cache;
    Segment left_segment_cache;
    Segment right_segment_cache;
    Segment front_segment_cache;
    Segment door_brake_segment_cache;
    Segment door_left_segment_cache;
    Segment door_right_segment_cache;
    Segment door_front_segment_cache;

    static Segment &rearSegment()
    {
        return strip.getSegment(REAR_SEGMENT);
        ;
    }
    static Segment &leftSegment()
    {
        return strip.getSegment(LEFT_SEGMENT);
    }
    static Segment &rightSegment()
    {
        return strip.getSegment(RIGHT_SEGMENT);
    }
    static Segment &frontSegment()
    {
        return strip.getSegment(FRONT_SEGMENT);
    }
    static Segment &doorBrakeSegment()
    {
        return strip.getSegment(REAR_SEGMENT);
    }
    static Segment &unifiedSegment()
    {
        return strip.getSegment(UNIFIED_SEGMENT);
    }
    static Segment &seg(uint8_t id)
    {
        return strip.getSegment(id);
    }
    // Declare the struct for PreviousState

    void setupSegments()
    {
        strip.setSegment(RIGHT_SEGMENT, RIGHT_SEGMENT_START, RIGHT_SEGMENT_END, 1, 0, 0);
        strip.setSegment(FRONT_SEGMENT, FRONT_SEGMENT_START, FRONT_SEGMENT_END, 1, 0, 0);
        strip.setSegment(LEFT_SEGMENT, LEFT_SEGMENT_START, LEFT_SEGMENT_END, 1, 0, 0);
        strip.setSegment(REAR_SEGMENT, REAR_SEGMENT_START, REAR_SEGMENT_END, 1, 0, 0);

        auto &front = frontSegment();
        auto &rear = rearSegment();
        auto &left = leftSegment();
        auto &right = rightSegment();

        right.setOption(SEG_OPTION_ON, false);
        right.setOption(SEG_OPTION_SELECTED, true);
        left.setOption(SEG_OPTION_ON, false);
        left.setOption(SEG_OPTION_SELECTED, true);
        rear.setOption(SEG_OPTION_ON, false);
        rear.setOption(SEG_OPTION_SELECTED, true);
        front.setOption(SEG_OPTION_ON, false);
        front.setOption(SEG_OPTION_SELECTED, true);

    }
    bool checkSegmentIntegrity()
    {
        static bool previousIntegrityCheckResult = true;

        bool integrityCheckResult = true;

        // Check if each segment is assigned to the correct LED start and end

        integrityCheckResult &= seg(REAR_SEGMENT).start == REAR_SEGMENT_START;
        integrityCheckResult &= seg(REAR_SEGMENT).stop == REAR_SEGMENT_END;
        integrityCheckResult &= seg(LEFT_SEGMENT).start == LEFT_SEGMENT_START;
        integrityCheckResult &= seg(LEFT_SEGMENT).stop == LEFT_SEGMENT_END;
        integrityCheckResult &= seg(FRONT_SEGMENT).start == FRONT_SEGMENT_START;
        integrityCheckResult &= seg(FRONT_SEGMENT).stop == FRONT_SEGMENT_END;
        integrityCheckResult &= seg(RIGHT_SEGMENT).start == RIGHT_SEGMENT_START;
        integrityCheckResult &= seg(RIGHT_SEGMENT).stop == RIGHT_SEGMENT_END;

        // Print all start and stop values to console.

        if (!integrityCheckResult && !previousIntegrityCheckResult)
        {
            Serial.println("Segment integrity check result: " + String(integrityCheckResult));
            Serial.println("REAR_SEGMENT_START: " + String(strip.getSegment(REAR_SEGMENT).start) + " REAR_SEGMENT_END: " + String(strip.getSegment(REAR_SEGMENT).stop) + "|" + String(REAR_SEGMENT_START) + ":" + String(REAR_SEGMENT_END));
            Serial.println("LEFT_SEGMENT_START: " + String(strip.getSegment(LEFT_SEGMENT).start) + " LEFT_SEGMENT_END: " + String(strip.getSegment(LEFT_SEGMENT).stop) + "|" + String(LEFT_SEGMENT_START) + ":" + String(LEFT_SEGMENT_END));
            Serial.println("FRONT_SEGMENT_START: " + String(strip.getSegment(FRONT_SEGMENT).start) + " FRONT_SEGMENT_END: " + String(strip.getSegment(FRONT_SEGMENT).stop) + "|" + String(FRONT_SEGMENT_START) + ":" + String(FRONT_SEGMENT_END));
            Serial.println("RIGHT_SEGMENT_START: " + String(strip.getSegment(RIGHT_SEGMENT).start) + " RIGHT_SEGMENT_END: " + String(strip.getSegment(RIGHT_SEGMENT).stop) + "|" + String(RIGHT_SEGMENT_START) + ":" + String(RIGHT_SEGMENT_END));
            Serial.println("Configuration incorrect, resetting segments...");
            setupSegments();
            return false;
        }
        previousIntegrityCheckResult = integrityCheckResult;
        return true;
    }

  
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
    bool leftIndicatorOn()
    {
        return left_indicator_change_to_on;
    }
    bool rightIndicatorOn()
    {
        return right_indicator_change_to_on;
    }
    bool doorOpen()
    {
        return door_change_to_on;
    }
    bool unlocked()
    {
        return doors_unlocked_change_to_on;
    }
    bool locked()
    {
        return doors_locked_change_to_on;
    }
    bool reverseEngaged()
    {
        return reverse_change_to_on;
    }
    bool ignitionOn()
    {
        return ignition_change_to_on;
    }

};
std::vector<int> DEFAULT_SEGMENT_IDS = {FRONT_SEGMENT, LEFT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT};

#endif // SUBARU_TELEMETRY_H
