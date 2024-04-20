#ifndef SEGCON_H
#define SEGCON_H

#include "wled.h"
#define REAR_SEGMENT 0
#define LEFT_SEGMENT 1
#define RIGHT_SEGMENT 2
#define FRONT_SEGMENT 3
#define REAR_LEFT_SEGMENT 4
#define REAR_RIGHT_SEGMENT 5
#define FRONT_RIGHT_SEGMENT 6
#define FRONT_LEFT_SEGMENT 7
#define SCOOP_SEGMENT 8
#define GRILL_SEGMENT 9

#define UNIFIED_SEGMENT 10

struct SubaruSegmentConfig
{
    int rearStart;
    int rearEnd;
    int leftStart;
    int leftEnd;
    int rightStart;
    int rightEnd;
    int frontStart;
    int frontEnd;

    int rearLeftDoorStart;
    int rearLeftDoorEnd;
    int rearLeftWellStart;
    int rearLeftWellEnd;

    int rearRightWellStart;
    int rearRightWellEnd;
    int rearRightDoorStart;
    int rearRightDoorEnd;

    int frontRightDoorStart;
    int frontRightDoorEnd;
    int frontRightWellStart;
    int frontRightWellEnd;

    int frontLeftWellStart;
    int frontLeftWellEnd;
    int frontLeftDoorStart;
    int frontLeftDoorEnd;

    int scoopStart;
    int scoopEnd;
    int grilleStart;
    int grilleEnd;
    int unifiedStart;
    int unifiedEnd;
};

#define INSTANT_TRANSITION 0
#define MEDIUM_TRANSITION 1000
#define SLOW_TRANSITION 2000
/**
 * Create a SubaruStripDimensions class that functions as a collection of segment attributes.
 * Each segment should have a position, length, start and end property.
 * The start and end positions should be calculated based on the length of the previous segment, and the position of the previous segment.
 */

class SubaruStripDimensions
{
public:
    int rearStart = 0;
    int rearEnd = 0;
    int leftStart = 0;
    int leftEnd = 0;
    int rightStart = 0;
    int rightEnd = 0;
    int frontStart = 0;
    int frontEnd = 0;
    int rearLeftStart = 0;
    int rearLeftEnd = 0;
    int rearRightStart = 0;
    int rearRightEnd = 0;
    int frontRightStart = 0;
    int frontRightEnd = 0;
    int frontLeftStart = 0;
    int frontLeftEnd = 0;
    int scoopStart = 0;
    int scoopEnd = 0;
    int grilleStart = 0;
    int grilleEnd = 0;
    int unifiedStart = 0;
    int unifiedEnd = 0;

    int rearLength = 65;       // 65 DATA 1 (IO15)
    int leftLength = 210;      // 275 DATA 1 (IO15)
    int rightLength = 210;     // 485 DATA 2 (IO18)
    int frontLength = 85;      // 570 DATA 2 (IO18)
    int rearLeftLength = 35;   // 605 DATA 3 (IO12)
    int rearRightLength = 35;  // 640 DATA 3 (IO12)
    int frontRightLength = 35; // 675 DATA 3 (IO12)
    int frontLeftLength = 35;  // 710 DATA 3 (IO12)
    int scoopLength = 30;      // 740 DATA 3 (IO12)
    int grilleLength = 40;     // 780 DATA 3 (IO12)

    /**
    DATA 1 = IO15 = 275
    DATA 2 = IO18 = 295
    DATA 3 = IO12 = 215
    */

    SubaruStripDimensions()
    {
        calculateStartEnd();
    }
    void updateDimensions()
    {
        calculateStartEnd();
    }
    void calculateStartEnd()
    {
        rearStart = 0;
        rearEnd = rearStart + rearLength;
        leftStart = rearEnd;
        leftEnd = leftStart + leftLength;
        rightStart = leftEnd;
        rightEnd = rightStart + rightLength;
        frontStart = rightEnd;
        frontEnd = frontStart + frontLength;
        rearLeftStart = frontEnd;
        rearLeftEnd = rearLeftStart + rearLeftLength;
        rearRightStart = rearLeftEnd;
        rearRightEnd = rearRightStart + rearRightLength;
        frontRightStart = rearRightEnd;
        frontRightEnd = frontRightStart + frontRightLength;
        frontLeftStart = frontRightEnd;
        frontLeftEnd = frontLeftStart + frontLeftLength;
        scoopStart = frontLeftEnd;
        scoopEnd = scoopStart + scoopLength;
        grilleStart = scoopEnd;
        grilleEnd = grilleStart + grilleLength;

        unifiedStart = 0;
        unifiedEnd = grilleEnd;
    }
    // Define the setters for all lengths, which also set the start/end values
    void setRearLength(int length)
    {
        rearLength = length;
    }
    void setLeftLength(int length)
    {
        leftLength = length;
    }
    void setRightLength(int length)
    {
        rightLength = length;
    }
    void setFrontLength(int length)
    {
        frontLength = length;
    }
    void setRearLeftLength(int length)
    {
        rearLeftLength = length;
    }
    void setRearRightLength(int length)
    {
        rearRightLength = length;
    }
    void setFrontRightLength(int length)
    {
        frontRightLength = length;
    }
    void setFrontLeftLength(int length)
    {
        frontLeftLength = length;
    }
    void setScoopLength(int length)
    {
        scoopLength = length;
    }
    void setGrilleLength(int length)
    {
        grilleLength = length;
    }
};

SubaruStripDimensions SUBARU_SEGMENT_CONFIG = SubaruStripDimensions();


std::vector<int> DEFAULT_INTERIOR_SEGMENT_IDS = {REAR_SEGMENT, RIGHT_SEGMENT, LEFT_SEGMENT, FRONT_SEGMENT, REAR_LEFT_SEGMENT, REAR_RIGHT_SEGMENT, FRONT_RIGHT_SEGMENT, FRONT_LEFT_SEGMENT, SCOOP_SEGMENT, GRILL_SEGMENT};


std::vector<int> DEFAULT_GROUND_SEGMENT_IDS = {REAR_SEGMENT, RIGHT_SEGMENT, LEFT_SEGMENT, FRONT_SEGMENT };

/**
 * Segment Controller Class
 */
class SegCon
{
public:
    SegCon()
    {
        if (strip.getLength() >= 300)
        {
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
    static Segment &rearLeftSegment()
    {
        return strip.getSegment(REAR_LEFT_SEGMENT);
    }
    static Segment &rearRightSegment()
    {
        return strip.getSegment(REAR_RIGHT_SEGMENT);
    }
    static Segment &frontRightSegment()
    {
        return strip.getSegment(FRONT_RIGHT_SEGMENT);
    }
    static Segment &frontLeftSegment()
    {
        return strip.getSegment(FRONT_LEFT_SEGMENT);
    }
    static Segment &scoopSegment()
    {
        return strip.getSegment(SCOOP_SEGMENT);
    }
    static Segment &grillSegment()
    {
        return strip.getSegment(GRILL_SEGMENT);
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
        if(strip.isUpdating()){
            Serial.println("Strip is updating, waiting for strip to finish updating...");
            return;
        }
        SUBARU_SEGMENT_CONFIG.updateDimensions();
        // Set segments for the right, front, left, rear, and unified segments

        Serial.println("Setting up rear segment [rearStart:" + String(SUBARU_SEGMENT_CONFIG.rearStart) + ", rearEnd:" + String(SUBARU_SEGMENT_CONFIG.rearEnd) + ", rearLenght:" + String(SUBARU_SEGMENT_CONFIG.rearLength) + "] ...");
        strip.setSegment(REAR_SEGMENT, SUBARU_SEGMENT_CONFIG.rearStart, SUBARU_SEGMENT_CONFIG.rearEnd, 1, 0, 0);
        Serial.println("Setting up left segment...");
        strip.setSegment(LEFT_SEGMENT, SUBARU_SEGMENT_CONFIG.leftStart, SUBARU_SEGMENT_CONFIG.leftEnd, 1, 0, 0);
        Serial.println("Setting up right segment [rightStart:" + String(SUBARU_SEGMENT_CONFIG.rightStart) + ", rightEnd:" + String(SUBARU_SEGMENT_CONFIG.rightEnd) + "] ...");
        strip.setSegment(RIGHT_SEGMENT, SUBARU_SEGMENT_CONFIG.rightStart, SUBARU_SEGMENT_CONFIG.rightEnd, 1, 0, 0);
        Serial.println("Setting up front segmen...");
        strip.setSegment(FRONT_SEGMENT, SUBARU_SEGMENT_CONFIG.frontStart, SUBARU_SEGMENT_CONFIG.frontEnd, 1, 0, 0);



        // Set segments for the newly added segments
        Serial.println("Setting up rear left segment...");
        strip.setSegment(REAR_LEFT_SEGMENT, SUBARU_SEGMENT_CONFIG.rearLeftStart, SUBARU_SEGMENT_CONFIG.rearLeftEnd, 1, 0, 0);
        Serial.println("Setting up rear right segment...");
        strip.setSegment(REAR_RIGHT_SEGMENT, SUBARU_SEGMENT_CONFIG.rearRightStart, SUBARU_SEGMENT_CONFIG.rearRightEnd, 1, 0, 0);
        Serial.println("Setting up front right segment...");
        strip.setSegment(FRONT_LEFT_SEGMENT, SUBARU_SEGMENT_CONFIG.frontLeftStart, SUBARU_SEGMENT_CONFIG.frontLeftEnd, 1, 0, 0);
        Serial.println("Setting up front left segment...");
        strip.setSegment(FRONT_RIGHT_SEGMENT, SUBARU_SEGMENT_CONFIG.frontRightStart, SUBARU_SEGMENT_CONFIG.frontRightEnd, 1, 0, 0);
        Serial.println("Setting up scoop segment...");
        strip.setSegment(SCOOP_SEGMENT, SUBARU_SEGMENT_CONFIG.scoopStart, SUBARU_SEGMENT_CONFIG.scoopEnd, 1, 0, 0);
        Serial.println("Setting up grill segment...");
        strip.setSegment(GRILL_SEGMENT, SUBARU_SEGMENT_CONFIG.grilleStart, SUBARU_SEGMENT_CONFIG.grilleEnd, 1, 0, 0);


        Serial.println("Setting up unified segment...");
        strip.setSegment(UNIFIED_SEGMENT, SUBARU_SEGMENT_CONFIG.unifiedStart, SUBARU_SEGMENT_CONFIG.unifiedEnd, 1, 0, 0);
        // Initial state settings for each segment can be set here

        auto &front = frontSegment();
        auto &rear = rearSegment();
        auto &left = leftSegment();
        auto &right = rightSegment();
        auto &rearLeft = rearLeftSegment();
        auto &rearRight = rearRightSegment();
        auto &frontRight = frontRightSegment();
        auto &frontLeft = frontLeftSegment();
        auto &scoop = scoopSegment();
        auto &grill = grillSegment();

        // Example setting options for segments
        right.setOption(SEG_OPTION_ON, false);
        right.setOption(SEG_OPTION_SELECTED, true);
        left.setOption(SEG_OPTION_ON, false);
        left.setOption(SEG_OPTION_SELECTED, true);
        rear.setOption(SEG_OPTION_ON, false);
        rear.setOption(SEG_OPTION_SELECTED, true);
        front.setOption(SEG_OPTION_ON, false);
        front.setOption(SEG_OPTION_SELECTED, true);
        rearLeft.setOption(SEG_OPTION_ON, false);
        rearLeft.setOption(SEG_OPTION_SELECTED, true);
        rearRight.setOption(SEG_OPTION_ON, false);
        rearRight.setOption(SEG_OPTION_SELECTED, true);
        frontRight.setOption(SEG_OPTION_ON, false);
        frontRight.setOption(SEG_OPTION_SELECTED, true);
        frontLeft.setOption(SEG_OPTION_ON, false);
        frontLeft.setOption(SEG_OPTION_SELECTED, true);
        scoop.setOption(SEG_OPTION_ON, false);
        scoop.setOption(SEG_OPTION_SELECTED, true);
        grill.setOption(SEG_OPTION_ON, false);
        grill.setOption(SEG_OPTION_SELECTED, true);
    }

    bool checkSegmentIntegrity()
    {
        // if (strip._segments.size() < 4)
        // {
        //     Serial.println("Segment size: " + String(strip._segments.size()) + " is less than 4, resetting segments...");
        //     return false;
        // }
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
        integrityCheckResult &= seg(REAR_LEFT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.rearLeftStart;
        integrityCheckResult &= seg(REAR_LEFT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.rearLeftEnd;
        integrityCheckResult &= seg(REAR_RIGHT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.rearRightStart;
        integrityCheckResult &= seg(REAR_RIGHT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.rearRightEnd;
        integrityCheckResult &= seg(FRONT_RIGHT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.frontRightStart;
        integrityCheckResult &= seg(FRONT_RIGHT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.frontRightEnd;
        integrityCheckResult &= seg(FRONT_LEFT_SEGMENT).start == SUBARU_SEGMENT_CONFIG.frontLeftStart;
        integrityCheckResult &= seg(FRONT_LEFT_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.frontLeftEnd;
        integrityCheckResult &= seg(SCOOP_SEGMENT).start == SUBARU_SEGMENT_CONFIG.scoopStart;
        integrityCheckResult &= seg(SCOOP_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.scoopEnd;
        integrityCheckResult &= seg(GRILL_SEGMENT).start == SUBARU_SEGMENT_CONFIG.grilleStart;
        integrityCheckResult &= seg(GRILL_SEGMENT).stop == SUBARU_SEGMENT_CONFIG.grilleEnd;

        // Print all start and stop values to console.

        if (!integrityCheckResult && !previousIntegrityCheckResult)
        {
            Serial.println("Segment integrity check result: " + String(integrityCheckResult));
            Serial.println("REAR_SEGMENT_START: " + String(strip.getSegment(REAR_SEGMENT).start) + " REAR_SEGMENT_END: " + String(strip.getSegment(REAR_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearEnd));
            Serial.println("LEFT_SEGMENT_START: " + String(strip.getSegment(LEFT_SEGMENT).start) + " LEFT_SEGMENT_END: " + String(strip.getSegment(LEFT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.leftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.leftEnd));
            Serial.println("FRONT_SEGMENT_START: " + String(strip.getSegment(FRONT_SEGMENT).start) + " FRONT_SEGMENT_END: " + String(strip.getSegment(FRONT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontEnd));
            Serial.println("RIGHT_SEGMENT_START: " + String(strip.getSegment(RIGHT_SEGMENT).start) + " RIGHT_SEGMENT_END: " + String(strip.getSegment(RIGHT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rightEnd));
            Serial.println("REAR_LEFT_SEGMENT_START: " + String(strip.getSegment(REAR_LEFT_SEGMENT).start) + " REAR_LEFT_SEGMENT_END: " + String(strip.getSegment(REAR_LEFT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearLeftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearLeftEnd));
            Serial.println("REAR_RIGHT_SEGMENT_START: " + String(strip.getSegment(REAR_RIGHT_SEGMENT).start) + " REAR_RIGHT_SEGMENT_END: " + String(strip.getSegment(REAR_RIGHT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearRightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearRightEnd));
            Serial.println("FRONT_RIGHT_SEGMENT_START: " + String(strip.getSegment(FRONT_RIGHT_SEGMENT).start) + " FRONT_RIGHT_SEGMENT_END: " + String(strip.getSegment(FRONT_RIGHT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontRightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontRightEnd));
            Serial.println("FRONT_LEFT_SEGMENT_START: " + String(strip.getSegment(FRONT_LEFT_SEGMENT).start) + " FRONT_LEFT_SEGMENT_END: " + String(strip.getSegment(FRONT_LEFT_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontLeftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontLeftEnd));
            Serial.println("SCOOP_SEGMENT_START: " + String(strip.getSegment(SCOOP_SEGMENT).start) + " SCOOP_SEGMENT_END: " + String(strip.getSegment(SCOOP_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.scoopStart) + ":" + String(SUBARU_SEGMENT_CONFIG.scoopEnd));
            Serial.println("GRILL_SEGMENT_START: " + String(strip.getSegment(GRILL_SEGMENT).start) + " GRILL_SEGMENT_END: " + String(strip.getSegment(GRILL_SEGMENT).stop) + "|" + String(SUBARU_SEGMENT_CONFIG.grilleStart) + ":" + String(SUBARU_SEGMENT_CONFIG.grilleEnd));
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