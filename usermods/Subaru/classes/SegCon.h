#ifndef SEGCON_H
#define SEGCON_H

#include "wled.h"

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
std::vector<int> DEFAULT_SEGMENT_IDS = {FRONT_SEGMENT, LEFT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT};


/** 
 * Segment Controller Class
*/
class SegCon
{
public:
    SegCon(){
        setupSegments();
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
};
SegCon SC;

#endif