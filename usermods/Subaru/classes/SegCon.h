#ifndef SEGCON_H
#define SEGCON_H

#include "wled.h"
#include "SubaruSegment.h"
#include "SubaruTelemetry.h"
#include "Ticker.h"

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

std::vector<int> ALL_SUBARU_SEGMENT_IDS = {REAR_SEGMENT, LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_LEFT_SEGMENT, REAR_RIGHT_SEGMENT, FRONT_RIGHT_SEGMENT, FRONT_LEFT_SEGMENT, SCOOP_SEGMENT, GRILL_SEGMENT};

std::vector<int> SUBARU_GROUND_SEGMENT_IDS = {REAR_SEGMENT, RIGHT_SEGMENT, LEFT_SEGMENT, FRONT_SEGMENT};

/**
 * Segment Controller Class
 */
class SegCon
{
private:
    static SegCon *instance;
    ColorPrint *p = ColorPrint::getInstance();
    SubaruTelemetry *ST = SubaruTelemetry::getInstance();

public:
    SubaruSegment rearSegment{REAR_SEGMENT};
    SubaruSegment leftSegment{LEFT_SEGMENT};
    SubaruSegment rightSegment{RIGHT_SEGMENT};
    SubaruSegment frontSegment{FRONT_SEGMENT};
    SubaruSegment rearLeftSegment{REAR_LEFT_SEGMENT};
    SubaruSegment rearRightSegment{REAR_RIGHT_SEGMENT};
    SubaruSegment frontRightSegment{FRONT_RIGHT_SEGMENT};
    SubaruSegment frontLeftSegment{FRONT_LEFT_SEGMENT};
    SubaruSegment scoopSegment{SCOOP_SEGMENT};
    SubaruSegment grillSegment{GRILL_SEGMENT};
    SubaruSegment unifiedSegment{UNIFIED_SEGMENT};
    //Create a collection of all segments "allSegments"

    //Create a map of relays to segments
    std::map<PinState*, std::vector<SubaruSegment>> relaySegmentMap = {
        {&SubaruTelemetry::getInstance()->groundRelay, {rearSegment, leftSegment, rightSegment, frontSegment}},
        {&SubaruTelemetry::getInstance()->interiorRelay, {rearLeftSegment, rearRightSegment, frontRightSegment, frontLeftSegment}},
        {&SubaruTelemetry::getInstance()->engineRelay, {scoopSegment, grillSegment}}};
    void resetAnyEffects();
    SegCon() {}
    void initialize()
    {
        
        if (strip._segments.size() > 8)
        {
            setupSegments();
        }
        else
        {
            Serial.println("Strip does not have enough segments for Subaru, waiting ...");
        }
    }
    void checkRelaySegments(std::map<PinState *, bool> pendingRelayState){
        //p->println("RELAY STATUS", ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
    
        for (auto const &relaySegmentPair : relaySegmentMap)
        {
            PinState *relay = relaySegmentPair.first;
            
            std::vector<SubaruSegment> segments = relaySegmentPair.second;
            bool relayShouldBeOff = true;
            for (SubaruSegment segment : segments)
            {
                segment.updateFromStrip();
                //Check if segment is on or if the matching PinState from pendingRelayState is true
                if(segment.on || pendingRelayState[relay]){
                    relayShouldBeOff = false;
                    break;
                }
            }
            if(relayShouldBeOff && relay->isOutputActive()){
               //Turn off the relay after 30 seconds...
                //p->println("\t[" + relay->name + "] should be OFF", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
               relay->delayOff(30);
            }else if(!relayShouldBeOff){
                //p->println("\t[" + relay->name + "] should be ON", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
                relay->write(true);
            }
            bool allRelaysOff = ST->allRelaysOff();
            if(allRelaysOff){
                //Take a bigger step if all relays are off...
            }

        }
    }

    static SegCon *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new SegCon();
            instance->initialize();
            instance->initializeAllSegments();
        }
        return instance;
    }

    static std::vector<SubaruSegment> allSegments;

    static void initializeAllSegments()
    {
        if (instance != nullptr)
        {
            allSegments = {
                instance->rearSegment, instance->leftSegment, instance->rightSegment,
                instance->frontSegment, instance->rearLeftSegment, instance->rearRightSegment,
                instance->frontRightSegment, instance->frontLeftSegment, instance->scoopSegment,
                instance->grillSegment, instance->unifiedSegment};
        }
    }

    static SubaruSegment seg(uint8_t segmentId){
        return instance->getSegment(segmentId);
    }
    SubaruSegment getSegment(uint8_t segmentId)
    {
        switch (segmentId)
        {
        case REAR_SEGMENT:
            rearSegment.updateFromStrip();
            return rearSegment;
        case LEFT_SEGMENT:
            leftSegment.updateFromStrip();
            return leftSegment;
        case RIGHT_SEGMENT:
            rightSegment.updateFromStrip();
            return rightSegment;
        case FRONT_SEGMENT:
            frontSegment.updateFromStrip();
            return frontSegment;
        case REAR_LEFT_SEGMENT:
            rearLeftSegment.updateFromStrip();
            return rearLeftSegment;
        case REAR_RIGHT_SEGMENT:
            rearRightSegment.updateFromStrip();
            return rearRightSegment;
        case FRONT_RIGHT_SEGMENT:
            frontRightSegment.updateFromStrip();
            return frontRightSegment;
        case FRONT_LEFT_SEGMENT:
            frontLeftSegment.updateFromStrip();
            return frontLeftSegment;
        case SCOOP_SEGMENT:
            scoopSegment.updateFromStrip();
            return scoopSegment;
        case GRILL_SEGMENT:
            grillSegment.updateFromStrip();
            return grillSegment;
        default:
            unifiedSegment.updateFromStrip();
            return unifiedSegment;
        }
    }

    // Declare the struct for PreviousState

    void setupSegments()
    {

        if (strip.isUpdating())
        {
            Serial.println("Strip is updating, waiting for strip to finish updating...");
            return;
        }
        SUBARU_SEGMENT_CONFIG.updateDimensions();
        // Set segments for the right, front, left, rear, and unified segments

        Serial.println("Setting up rear segment [rearStart:" + String(SUBARU_SEGMENT_CONFIG.rearStart) + ", rearEnd:" + String(SUBARU_SEGMENT_CONFIG.rearEnd) + ", rearLenght:" + String(SUBARU_SEGMENT_CONFIG.rearLength) + "] ...");
        rearSegment
            .setRelay(ST->groundRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.rearStart, SUBARU_SEGMENT_CONFIG.rearEnd);

        Serial.println("Setting up left segment...");
        leftSegment
            .setRelay(ST->groundRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.leftStart, SUBARU_SEGMENT_CONFIG.leftEnd);

        Serial.println("Setting up right segment [rightStart:" + String(SUBARU_SEGMENT_CONFIG.rightStart) + ", rightEnd:" + String(SUBARU_SEGMENT_CONFIG.rightEnd) + "] ...");
        rightSegment
            .setRelay(ST->groundRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.rightStart, SUBARU_SEGMENT_CONFIG.rightEnd);

        Serial.println("Setting up front segmen...");
        frontSegment
            .setRelay(ST->groundRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.frontStart, SUBARU_SEGMENT_CONFIG.frontEnd);

        // Set segments for the newly added segments
        Serial.println("Setting up rear left segment...");
        rearLeftSegment
            .setRelay(ST->interiorRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.rearLeftStart, SUBARU_SEGMENT_CONFIG.rearLeftEnd);

        Serial.println("Setting up rear right segment...");
        rearRightSegment
            .setRelay(ST->interiorRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.rearRightStart, SUBARU_SEGMENT_CONFIG.rearRightEnd);

        Serial.println("Setting up front right segment...");
        frontRightSegment
            .setRelay(ST->interiorRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.frontRightStart, SUBARU_SEGMENT_CONFIG.frontRightEnd);

        Serial.println("Setting up front left segment...");
        frontLeftSegment
            .setRelay(ST->interiorRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.frontLeftStart, SUBARU_SEGMENT_CONFIG.frontLeftEnd);

        Serial.println("Setting up scoop segment...");
        scoopSegment
            .setRelay(ST->engineRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.scoopStart, SUBARU_SEGMENT_CONFIG.scoopEnd);

        Serial.println("Setting up grill segment...");
        grillSegment
            .setRelay(ST->engineRelay)
            .setBounds(SUBARU_SEGMENT_CONFIG.grilleStart, SUBARU_SEGMENT_CONFIG.grilleEnd);

        Serial.println("Setting up unified segment...");
        unifiedSegment.setBounds(SUBARU_SEGMENT_CONFIG.unifiedStart, SUBARU_SEGMENT_CONFIG.unifiedEnd);
        // Initial state settings for each segment can be set here

        // Example setting options for segments
        rightSegment.setOption(SEG_OPTION_ON, false);
        rightSegment.setOption(SEG_OPTION_SELECTED, true);
        leftSegment.setOption(SEG_OPTION_ON, false);
        leftSegment.setOption(SEG_OPTION_SELECTED, true);
        rearSegment.setOption(SEG_OPTION_ON, false);
        rearSegment.setOption(SEG_OPTION_SELECTED, true);
        frontSegment.setOption(SEG_OPTION_ON, false);
        frontSegment.setOption(SEG_OPTION_SELECTED, true);
        rearLeftSegment.setOption(SEG_OPTION_ON, false);
        rearLeftSegment.setOption(SEG_OPTION_SELECTED, true);
        rearRightSegment.setOption(SEG_OPTION_ON, false);
        rearRightSegment.setOption(SEG_OPTION_SELECTED, true);
        frontRightSegment.setOption(SEG_OPTION_ON, false);
        frontRightSegment.setOption(SEG_OPTION_SELECTED, true);
        frontLeftSegment.setOption(SEG_OPTION_ON, false);
        frontLeftSegment.setOption(SEG_OPTION_SELECTED, true);
        scoopSegment.setOption(SEG_OPTION_ON, false);
        scoopSegment.setOption(SEG_OPTION_SELECTED, true);
        grillSegment.setOption(SEG_OPTION_ON, false);
        grillSegment.setOption(SEG_OPTION_SELECTED, true);
        resetAnyEffects();
    }

    bool checkSegmentIntegrity()
    {

        //static bool previousIntegrityCheckResult = true;

        bool integrityCheckResult = true;

        // Check if each segment is assigned to the correct LED start and end

        integrityCheckResult &= rearSegment.start == SUBARU_SEGMENT_CONFIG.rearStart;
        integrityCheckResult &= rearSegment.stop == SUBARU_SEGMENT_CONFIG.rearEnd;
        integrityCheckResult &= leftSegment.start == SUBARU_SEGMENT_CONFIG.leftStart;
        integrityCheckResult &= leftSegment.stop == SUBARU_SEGMENT_CONFIG.leftEnd;
        integrityCheckResult &= rightSegment.start == SUBARU_SEGMENT_CONFIG.rightStart;
        integrityCheckResult &= rightSegment.stop == SUBARU_SEGMENT_CONFIG.rightEnd;
        integrityCheckResult &= frontSegment.start == SUBARU_SEGMENT_CONFIG.frontStart;
        integrityCheckResult &= frontSegment.stop == SUBARU_SEGMENT_CONFIG.frontEnd;
        integrityCheckResult &= rearLeftSegment.start == SUBARU_SEGMENT_CONFIG.rearLeftStart;
        integrityCheckResult &= rearLeftSegment.stop == SUBARU_SEGMENT_CONFIG.rearLeftEnd;
        integrityCheckResult &= rearRightSegment.start == SUBARU_SEGMENT_CONFIG.rearRightStart;
        integrityCheckResult &= rearRightSegment.stop == SUBARU_SEGMENT_CONFIG.rearRightEnd;
        integrityCheckResult &= frontRightSegment.start == SUBARU_SEGMENT_CONFIG.frontRightStart;
        integrityCheckResult &= frontRightSegment.stop == SUBARU_SEGMENT_CONFIG.frontRightEnd;
        integrityCheckResult &= frontLeftSegment.start == SUBARU_SEGMENT_CONFIG.frontLeftStart;
        integrityCheckResult &= frontLeftSegment.stop == SUBARU_SEGMENT_CONFIG.frontLeftEnd;
        integrityCheckResult &= scoopSegment.start == SUBARU_SEGMENT_CONFIG.scoopStart;
        integrityCheckResult &= scoopSegment.stop == SUBARU_SEGMENT_CONFIG.scoopEnd;
        integrityCheckResult &= grillSegment.start == SUBARU_SEGMENT_CONFIG.grilleStart;
        integrityCheckResult &= grillSegment.stop == SUBARU_SEGMENT_CONFIG.grilleEnd;

        // Print all start and stop values to console.

        if (!integrityCheckResult)
        {
            Serial.println("Segment integrity check result: " + String(integrityCheckResult));
            Serial.println("REAR_SEGMENT_START: " + String(rearSegment.start) + " REAR_SEGMENT_END: " + String(rearSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearEnd));
            Serial.println("LEFT_SEGMENT_START: " + String(leftSegment.start) + " LEFT_SEGMENT_END: " + String(leftSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.leftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.leftEnd));
            Serial.println("FRONT_SEGMENT_START: " + String(frontSegment.start) + " FRONT_SEGMENT_END: " + String(frontSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontEnd));
            Serial.println("RIGHT_SEGMENT_START: " + String(rightSegment.start) + " RIGHT_SEGMENT_END: " + String(rightSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rightEnd));
            Serial.println("REAR_LEFT_SEGMENT_START: " + String(rearLeftSegment.start) + " REAR_LEFT_SEGMENT_END: " + String(rearLeftSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearLeftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearLeftEnd));
            Serial.println("REAR_RIGHT_SEGMENT_START: " + String(rearRightSegment.start) + " REAR_RIGHT_SEGMENT_END: " + String(rearRightSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.rearRightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.rearRightEnd));
            Serial.println("FRONT_RIGHT_SEGMENT_START: " + String(frontRightSegment.start) + " FRONT_RIGHT_SEGMENT_END: " + String(frontRightSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontRightStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontRightEnd));
            Serial.println("FRONT_LEFT_SEGMENT_START: " + String(frontLeftSegment.start) + " FRONT_LEFT_SEGMENT_END: " + String(frontLeftSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.frontLeftStart) + ":" + String(SUBARU_SEGMENT_CONFIG.frontLeftEnd));
            Serial.println("SCOOP_SEGMENT_START: " + String(scoopSegment.start) + " SCOOP_SEGMENT_END: " + String(scoopSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.scoopStart) + ":" + String(SUBARU_SEGMENT_CONFIG.scoopEnd));
            Serial.println("GRILL_SEGMENT_START: " + String(grillSegment.start) + " GRILL_SEGMENT_END: " + String(grillSegment.stop) + "|" + String(SUBARU_SEGMENT_CONFIG.grilleStart) + ":" + String(SUBARU_SEGMENT_CONFIG.grilleEnd));
            Serial.println("Configuration incorrect, resetting segments...");
            setupSegments();
            return false;
        }
        //previousIntegrityCheckResult = integrityCheckResult;
        return true;
    }
};
SegCon *SegCon::instance = nullptr; // Initialize the static instance variable
std::vector<SubaruSegment> SegCon::allSegments;


void SubaruTelemetry::turnOnRelay(int segmentID){
    SegCon::seg(segmentID).activateRelay();
}

void SubaruTelemetry::turnOffRelay(int segmentID){
    SegCon::seg(segmentID).deactivateRelay();
}
void SubaruTelemetry::relayOffCallback(PinState *relay) {
    //p->println("THE CALLBACK IS FIRING!!!!!", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
    relay->write(false);
}

void SubaruTelemetry::turnOnRelay(PinState *relay){
    relay->write(true);
    //relay.write(false);
}
#endif