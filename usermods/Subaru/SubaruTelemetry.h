

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
public:
    // Declare all the variables and members that need to be initialized
    /**
     *  Constructor method SubaruTelemetry()
     */
    SubaruTelemetry()
    {
        // Use default values to set up the telemetry
        setupSegments();
    }

    static const int SDA_PIN = 21; // Make these constants static
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
    bool unified = false;
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
    static Segment &rearSegment()
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
    struct PreviousState
    {
        uint8_t mode;
        uint8_t palette;
        uint8_t intensity;
        uint32_t *colors;
        uint8_t speed;
        uint8_t opacity;
        uint8_t cct;
    } previousLeftState, previousRightState, previousBrakeState, previousFrontState;

    void unifiedMode(){
        strip.purgeSegments(true);
        strip.setSegment(UNIFIED_SEGMENT, SUBARU_SEGMENT_CONFIG.unifiedStart, SUBARU_SEGMENT_CONFIG.unifiedEnd, 1, 0, 0);
        auto &unified = unifiedSegment();
        unified.setOption(SEG_OPTION_ON, false);
        unified.setOption(SEG_OPTION_SELECTED, true);
    }
    void segmentedMode(){
        strip.setSegment(REAR_SEGMENT, SUBARU_SEGMENT_CONFIG.rearStart, SUBARU_SEGMENT_CONFIG.rearEnd, 1, 0, 0);
        strip.setSegment(RIGHT_SEGMENT, SUBARU_SEGMENT_CONFIG.rightStart, SUBARU_SEGMENT_CONFIG.rightEnd, 1, 0, 0);
        strip.setSegment(FRONT_SEGMENT, SUBARU_SEGMENT_CONFIG.frontStart, SUBARU_SEGMENT_CONFIG.frontEnd, 1, 0, 0);
        strip.setSegment(LEFT_SEGMENT, SUBARU_SEGMENT_CONFIG.leftStart, SUBARU_SEGMENT_CONFIG.leftEnd, 1, 0, 0);
        
        auto &rear = rearSegment();
        auto &right = rightSegment();
        auto &front = frontSegment();
        auto &left = leftSegment();

        rear.setOption(SEG_OPTION_ON, false);
        rear.setOption(SEG_OPTION_SELECTED, true);
        right.setOption(SEG_OPTION_ON, false);
        right.setOption(SEG_OPTION_SELECTED, true);
        front.setOption(SEG_OPTION_ON, false);
        front.setOption(SEG_OPTION_SELECTED, true);
        left.setOption(SEG_OPTION_ON, false);
        left.setOption(SEG_OPTION_SELECTED, true);

    }
    void setupSegments()
    {
        if(unified){
            unifiedMode();
        }else{
            segmentedMode();
        }
    }

    bool checkSegmentIntegrity()
    {  
        if(strip.isUpdating()){
            return false;
        }
        static bool previousIntegrityCheckResult = true;

        bool integrityCheckResult = true;

        //Check if each segment is a nullptr and if it is active
        if(unified){
            if(seg(REAR_SEGMENT).isActive()){
                integrityCheckResult &= seg(REAR_SEGMENT).start == SUBARU_SEGMENT_CONFIG.rearStart;
            }
            if(seg(LEFT_SEGMENT).isActive()){
                integrityCheckResult &= seg(LEFT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.leftStart;
            }
            if(seg(FRONT_SEGMENT).isActive()){
                integrityCheckResult &= seg(FRONT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.frontStart;
            }
            if(seg(RIGHT_SEGMENT).isActive()){
                integrityCheckResult &= seg(RIGHT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.rightStart;
            }
            if(seg(UNIFIED_SEGMENT).isActive()){
                integrityCheckResult &= seg(UNIFIED_SEGMENT).start == SUBARU_SEGMENT_CONFIG.unifiedStart;
            }
            if(seg(REAR_SEGMENT).isActive()){
                integrityCheckResult &= seg(REAR_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.rearEnd;
            }
            if(seg(LEFT_SEGMENT).isActive()){
                integrityCheckResult &= seg(LEFT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.leftEnd;
            }
            if(seg(FRONT_SEGMENT).isActive()){
                integrityCheckResult &= seg(FRONT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.frontEnd;
            }
            if(seg(RIGHT_SEGMENT).isActive()){
                integrityCheckResult &= seg(RIGHT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.rightEnd;
            }
        }else{
            integrityCheckResult &= seg(UNIFIED_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.unifiedEnd;
        }

        
        // Print all start and stop values to console.

        if (!integrityCheckResult && !previousIntegrityCheckResult)
        {
            
            Serial.println("Segment integrity check result: " + String(integrityCheckResult));
            if(!unified){
                if(seg(REAR_SEGMENT).isActive()){
                    Serial.println("REAR_SEGMENT_START: " + String(seg(REAR_SEGMENT).start) + " REAR_SEGMENT_END: " + String(seg(REAR_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearEnd));
                }
                if(seg(LEFT_SEGMENT).isActive()){
                    Serial.println("LEFT_SEGMENT_START: " + String(seg(LEFT_SEGMENT).start) + " LEFT_SEGMENT_END: " + String(seg(LEFT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.leftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.leftEnd));
                }
                if(seg(FRONT_SEGMENT).isActive()){
                    Serial.println("FRONT_SEGMENT_START: " + String(seg(FRONT_SEGMENT).start) + " FRONT_SEGMENT_END: " + String(seg(FRONT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontEnd));
                }
                if(seg(RIGHT_SEGMENT).isActive()){
                    Serial.println("RIGHT_SEGMENT_START: " + String(seg(RIGHT_SEGMENT).start) + " RIGHT_SEGMENT_END: " + String(seg(RIGHT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rightEnd));
                }
            }else{
                Serial.println("UNIFIED_SEGMENT_START: " + String(seg(UNIFIED_SEGMENT).start) + " UNIFIED_SEGMENT_END: " + String(seg(UNIFIED_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.unifiedStart) + ":" + String(SUBARU_SEGMENT_CONFIG.unifiedEnd));
            }
            Serial.println("Configuration incorrect, resetting segments...");
            setupSegments();
            return false;
        }
        previousIntegrityCheckResult = integrityCheckResult;
        return true;
    }

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
    }
};

#endif // SUBARU_TELEMETRY_H
