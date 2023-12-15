#ifndef SEGCON_H
#define SEGCON_H

#include "wled.h"

#define RIGHT_SEGMENT 0
#define FRONT_SEGMENT 1
#define LEFT_SEGMENT 2
#define REAR_SEGMENT 3

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
std::vector<int> DEFAULT_SEGMENT_IDS = {FRONT_SEGMENT, LEFT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT};


/** 
 * Segment Controller Class
*/
class SegCon
{
public:
    SegCon(){
        if (strip.getLength() >= 300) {
            setupSegments();
        }
    }
    static Segment &rearSegment()
    {
        return strip.getSegment(REAR_SEGMENT);
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
        strip.setSegment(RIGHT_SEGMENT, SUBARU_SEGMENT_CONFIG.rightStart, SUBARU_SEGMENT_CONFIG.rightEnd, 1, 0, 0);
        strip.setSegment(FRONT_SEGMENT, SUBARU_SEGMENT_CONFIG.frontStart, SUBARU_SEGMENT_CONFIG.frontEnd, 1, 0, 0);
        strip.setSegment(LEFT_SEGMENT, SUBARU_SEGMENT_CONFIG.leftStart, SUBARU_SEGMENT_CONFIG.leftEnd, 1, 0, 0);
        strip.setSegment(REAR_SEGMENT, SUBARU_SEGMENT_CONFIG.rearStart, SUBARU_SEGMENT_CONFIG.rearEnd, 1, 0, 0);
        strip.setSegment(UNIFIED_SEGMENT, SUBARU_SEGMENT_CONFIG.unifiedStart, SUBARU_SEGMENT_CONFIG.unifiedEnd, 1, 0, 0);


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
        if(strip._segments.size() < 4) return false;
        static bool previousIntegrityCheckResult = true;

        bool integrityCheckResult = true;

        // Check if each segment is assigned to the correct LED start and end

        integrityCheckResult &= seg(REAR_SEGMENT).start == SUBARU_SEGMENT_CONFIG.rearStart;
        integrityCheckResult &= seg(REAR_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.rearEnd;
        integrityCheckResult &= seg(LEFT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.leftStart;
        integrityCheckResult &= seg(LEFT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.leftEnd;
        integrityCheckResult &= seg(FRONT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.frontStart;
        integrityCheckResult &= seg(FRONT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.frontEnd;
        integrityCheckResult &= seg(RIGHT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.rightStart;
        integrityCheckResult &= seg(RIGHT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.rightEnd;

        // Print all start and stop values to console.

        if (!integrityCheckResult && !previousIntegrityCheckResult)
        {
            Serial.println("Segment integrity check result: " + String(integrityCheckResult));
            Serial.println("REAR_SEGMENT_START: " + String(strip.getSegment(REAR_SEGMENT).start) + " REAR_SEGMENT_END: " + String(strip.getSegment(REAR_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearEnd));
            Serial.println("LEFT_SEGMENT_START: " + String(strip.getSegment(LEFT_SEGMENT).start) + " LEFT_SEGMENT_END: " + String(strip.getSegment(LEFT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.leftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.leftEnd));
            Serial.println("FRONT_SEGMENT_START: " + String(strip.getSegment(FRONT_SEGMENT).start) + " FRONT_SEGMENT_END: " + String(strip.getSegment(FRONT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontEnd));
            Serial.println("RIGHT_SEGMENT_START: " + String(strip.getSegment(RIGHT_SEGMENT).start) + " RIGHT_SEGMENT_END: " + String(strip.getSegment(RIGHT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rightEnd));
            Serial.println("Configuration incorrect, resetting segments...");
            setupSegments();
            return false;
        }
        previousIntegrityCheckResult = integrityCheckResult;
        return true;
    }
};
SegCon SC;

#endif