#ifndef SUBARU_SEGMENT_H
#define SUBARU_SEGMENT_H

#include "wled.h"
#include "PinState.h"
#include "ColorPrint.h"

class SubaruSegment : public Segment
{
private:
  ColorPrint *p = ColorPrint::getInstance();

public:
    uint8_t id; // Identifier for convenience
    PinState *relay; // Pointer to PinState for controlling a relay

    // Constructor that initializes a SubaruSegment with a segment ID
    SubaruSegment(uint8_t segmentId)
    : Segment(strip.getSegment(segmentId)), // Initialize the base Segment class
      id(segmentId), // Initialize id next as it appears before relay in the declaration
      relay(nullptr) // Initialize relay last
    {
        // Additional initialization or actions can be done here
    }

    void setBounds(uint16_t start, uint16_t end)
    {
        Serial.println("Setting bounds for segment " + String(id) + " from " + String(start) + " to " + String(end));
        //delay(1000);
        strip.setSegment(id, start, end, 1, 0, 0);
        updateFromStrip();
    }

    // Additional methods to augment the segment's functionality can be added here
    void activateRelay()
    {
        if (relay && !relay->isOutputActive())
        {
            relay->write(true);
        }else if(relay->isOutputActive()){

        }
    }

    void deactivateRelay()
    {
        if (relay && relay->isOutputActive())
        {
            relay->write(false);
        }else if(!relay->isOutputActive()){
            Serial.println("======================================");
            Serial.println("Relay already off for segment " + String(id) + "!"); 
            Serial.println("======================================");
        }
    }
    void updateFromStrip() {
        Segment current = strip.getSegment(id);
        // Update SubaruSegment's internal state to match 'current'
        // Assuming Segment has methods or properties that you need; adjust as necessary
        // For example:
        this->start = current.start;
        this->stop = current.stop;
        this->options = current.options;
        this->on = current.on;
        // Add more fields as required
    }
    // Example setter for the relay
    SubaruSegment &setRelay(PinState &relayState)
    {
        relay = &relayState;
        return *this;
    }
};

#endif // SUBARU_SEGMENT_H
