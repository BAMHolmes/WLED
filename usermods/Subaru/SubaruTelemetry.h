

#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>

#ifndef SUBARU_TELEMETRY_H
#define SUBARU_TELEMETRY_H


// #define BRAKE_PEDAL 19
// #define DOOR_OPEN 5
// #define LEFT_INDICATOR 23
// #define RIGHT_INDICATOR 18
#define BRAKE_SEGMENT 0
#define LEFT_SEGMENT 1
#define FRONT_SEGMENT 2
#define RIGHT_SEGMENT 3

#define RIGHT_SEGMENT_START 0
#define RIGHT_SEGMENT_END 200
#define FRONT_SEGMENT_START 200
#define FRONT_SEGMENT_END 240
#define LEFT_SEGMENT_START 240
#define LEFT_SEGMENT_END 440
#define BRAKE_SEGMENT_START 440
#define BRAKE_SEGMENT_END 480

#define INSTANT_TRANSITION 0
#define MEDIUM_TRANSITION 1000
#define SLOW_TRANSITION 2000
class SubaruTelemetry {
public:

    // Declare all the variables and members that need to be initialized

    static const int SDA_PIN = 21;  // Make these constants static
    static const int SCL_PIN = 22;
    static const int PCF8575_ADDRESS = 0x20;

    bool paused = false;
    bool flashingRed = false;
    bool welcomeLights = false;
    bool left_indicator_on = false;
    bool right_indicator_on = false;
    bool brake_pedal_pressed = false;
    bool left_indicator_previous_state = false;
    bool right_indicator_previous_state = false;
    bool brake_pedal_previous_state = false;
    bool reverse_previous_state = false;
    bool door_previous_state = false;
    bool override = false;
    bool door_is_open = false;
    bool doors_unlocked = false;
    bool doors_locked = false;
    bool car_in_reverse = false;
    bool ignition = true;
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

    Segment brake_segment;
    Segment left_segment;
    Segment right_segment;
    Segment front_segment;

    // Declare the struct for PreviousState
    struct PreviousState {
        uint8_t mode;
        uint8_t palette;
        uint8_t intensity;
        uint32_t* colors;
        uint8_t speed;
        uint8_t opacity;
        uint8_t cct;
    } previousLeftState, previousRightState, previousBrakeState, previousFrontState;

    void setupSegments(){
    
      strip.setSegment(BRAKE_SEGMENT, BRAKE_SEGMENT_START, BRAKE_SEGMENT_END, 1, 0, 0);

      strip.setSegment(BRAKE_SEGMENT, BRAKE_SEGMENT_START, BRAKE_SEGMENT_END, 1, 0, 0);
      strip.setSegment(LEFT_SEGMENT, LEFT_SEGMENT_START, LEFT_SEGMENT_END, 1, 0, 0);
      strip.setSegment(FRONT_SEGMENT, FRONT_SEGMENT_START, FRONT_SEGMENT_END, 1, 0, 0);
      strip.setSegment(RIGHT_SEGMENT, RIGHT_SEGMENT_START, RIGHT_SEGMENT_END, 1, 0, 0);
      
      right_segment = right_segment_cache = door_right_segment_cache = strip.getSegment(RIGHT_SEGMENT);
      left_segment = left_segment_cache = door_left_segment_cache = strip.getSegment(LEFT_SEGMENT);
      brake_segment = brake_segment_cache = door_brake_segment_cache = strip.getSegment(BRAKE_SEGMENT);
      front_segment = front_segment_cache = door_front_segment_cache = strip.getSegment(FRONT_SEGMENT);


      right_segment.setOption(SEG_OPTION_ON, 1);
      right_segment.setOption(SEG_OPTION_SELECTED, 1);
      left_segment.setOption(SEG_OPTION_ON, 1);
      left_segment.setOption(SEG_OPTION_SELECTED, 1);
      brake_segment.setOption(SEG_OPTION_ON, 1);
      brake_segment.setOption(SEG_OPTION_SELECTED, 1);
      front_segment.setOption(SEG_OPTION_ON, 1);
      front_segment.setOption(SEG_OPTION_SELECTED, 1);

      /** Set the name/title of segments */
      right_segment.name = (char*)"Right Strip";
      left_segment.name = (char*)"Left Strip";
      brake_segment.name = (char*)"Brake Strip";
      front_segment.name = (char*)"Front Strip";

    }
    void checkSegmentIntegrity() {
        static bool previousIntegrityCheckResult = true;

        bool integrityCheckResult = true;

        // Check if each segment is assigned to the correct LED start and end
        integrityCheckResult &= strip.getSegment(BRAKE_SEGMENT).start == BRAKE_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(BRAKE_SEGMENT).stop == BRAKE_SEGMENT_END;
        integrityCheckResult &= strip.getSegment(LEFT_SEGMENT).start == LEFT_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(LEFT_SEGMENT).stop == LEFT_SEGMENT_END;
        integrityCheckResult &= strip.getSegment(FRONT_SEGMENT).start == FRONT_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(FRONT_SEGMENT).stop == FRONT_SEGMENT_END;
        integrityCheckResult &= strip.getSegment(RIGHT_SEGMENT).start == RIGHT_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(RIGHT_SEGMENT).stop == RIGHT_SEGMENT_END;

        if (!integrityCheckResult && previousIntegrityCheckResult) {
            Serial.println("Detected a change in segment integrity. Running setup() again.");
            setupSegments();
        }

        previousIntegrityCheckResult = integrityCheckResult;
    }

    void select(int segment)
    {
        right_segment.setOption(SEG_OPTION_SELECTED, 0);
        left_segment.setOption(SEG_OPTION_SELECTED, 0);
        brake_segment.setOption(SEG_OPTION_SELECTED, 0);
        front_segment.setOption(SEG_OPTION_SELECTED, 0);
        strip.getSegment(segment).setOption(SEG_OPTION_SELECTED, 1);
    }

    void selectAll()
    {
        right_segment.setOption(SEG_OPTION_SELECTED, 1);
        left_segment.setOption(SEG_OPTION_SELECTED, 1);
        brake_segment.setOption(SEG_OPTION_SELECTED, 1);
        front_segment.setOption(SEG_OPTION_SELECTED, 1);
    }

    uint16_t readPCF8575() {
        uint16_t data = 0;
        Wire.beginTransmission(PCF8575_ADDRESS);  // Begin transmission to PCF8575
        Wire.endTransmission();  // End transmission

        Wire.requestFrom(PCF8575_ADDRESS, 2);  // Request 2 bytes from PCF8575
        if (Wire.available()) {
            data = Wire.read();           // Read the low byte
            data |= Wire.read() << 8;     // Read the high byte and shift it left
        }
        return data;
    }


    void readTelemetry(){

        uint16_t pinState = readPCF8575();  // Read the state of all pins

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
    }


    

};

#endif // SUBARU_TELEMETRY_H
