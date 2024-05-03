#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>
#include "classes/QueueManager.h"
#include <chrono>

const char UM_REAR_SEGMENT_LENGTH[] PROGMEM = "rearSegmentLength";
const char UM_RIGHT_SEGMENT_LENGTH[] PROGMEM = "rightSegmentLength";
const char UM_LEFT_SEGMENT_LENGTH[] PROGMEM = "leftSegmentLength";
const char UM_FRONT_SEGMENT_LENGTH[] PROGMEM = "frontSegmentLength";
const char UM_REAR_LEFT_SEGMENT_LENGTH[] PROGMEM = "rearLeftSegmentLength";
const char UM_REAR_RIGHT_SEGMENT_LENGTH[] PROGMEM = "rearRightSegmentLength";
const char UM_FRONT_RIGHT_SEGMENT_LENGTH[] PROGMEM = "frontRightSegmentLength";
const char UM_FRONT_LEFT_SEGMENT_LENGTH[] PROGMEM = "frontLeftSegmentLength";
const char UM_SCOOP_SEGMENT_LENGTH[] PROGMEM = "scoopSegmentLength";
const char UM_GRILLE_SEGMENT_LENGTH[] PROGMEM = "grilleSegmentLength";

typedef std::pair<bool, PinState*> RelayState;
class Subaru : public Usermod

{
private:
  SubaruTelemetry *ST = SubaruTelemetry::getInstance();
  ColorPrint *p = ColorPrint::getInstance();
  QueueManager queueManager;
  std::map<PinState*, bool> pendingRelayState;
  // Usermod variable declarations for storing segment indices
  std::chrono::steady_clock::time_point powerOffTimer;
  bool timerStarted;
  int powerOffDelay = 30;
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;

  void addToConfig(JsonObject &root) override
  {
    JsonObject top = root.createNestedObject(FPSTR(_name));
    top[FPSTR(_enabled)] = enabled;

    top[FPSTR(UM_REAR_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.rearLength;
    top[FPSTR(UM_LEFT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.leftLength;
    top[FPSTR(UM_RIGHT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.rightLength;
    top[FPSTR(UM_FRONT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.frontLength;
    top[FPSTR(UM_REAR_LEFT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.rearLeftLength;
    top[FPSTR(UM_REAR_RIGHT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.rearRightLength;
    top[FPSTR(UM_FRONT_RIGHT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.frontRightLength;
    top[FPSTR(UM_FRONT_LEFT_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.frontLeftLength;
    top[FPSTR(UM_SCOOP_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.scoopLength;
    top[FPSTR(UM_GRILLE_SEGMENT_LENGTH)] = SUBARU_SEGMENT_CONFIG.grilleLength;
  }

  bool readFromConfig(JsonObject &root) override
  {
    JsonObject top = root[FPSTR(_name)];
    bool configComplete = !top.isNull();
    //Serial.println("Before config read: rearLength=" + String(SUBARU_SEGMENT_CONFIG.rearLength));
    configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
    configComplete &= getJsonValue(top[FPSTR(UM_REAR_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.rearLength);
    configComplete &= getJsonValue(top[FPSTR(UM_LEFT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.leftLength);
    configComplete &= getJsonValue(top[FPSTR(UM_RIGHT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.rightLength);
    configComplete &= getJsonValue(top[FPSTR(UM_FRONT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.frontLength);
    configComplete &= getJsonValue(top[FPSTR(UM_REAR_LEFT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.rearLeftLength);
    configComplete &= getJsonValue(top[FPSTR(UM_REAR_RIGHT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.rearRightLength);
    configComplete &= getJsonValue(top[FPSTR(UM_FRONT_RIGHT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.frontRightLength);
    configComplete &= getJsonValue(top[FPSTR(UM_FRONT_LEFT_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.frontLeftLength);
    configComplete &= getJsonValue(top[FPSTR(UM_SCOOP_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.scoopLength);
    configComplete &= getJsonValue(top[FPSTR(UM_GRILLE_SEGMENT_LENGTH)], SUBARU_SEGMENT_CONFIG.grilleLength);
    //Serial.println("After config read: rearLength=" + String(SUBARU_SEGMENT_CONFIG.rearLength));

    if (configComplete)
    {
      SUBARU_SEGMENT_CONFIG.updateDimensions();
    }
    return configComplete;
  }
  const char *flashStringToChar(const __FlashStringHelper *flashString)
  {
    // You must ensure this buffer is large enough for your strings
    static char buffer[64];
    memcpy_P(buffer, reinterpret_cast<const char *>(flashString), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0; // Ensure null-terminated string
    return buffer;
  }
  void appendConfigData() override
  {
    String configName = String(FPSTR(_name));

    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_REAR_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Rear length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_LEFT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Left length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_RIGHT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Right length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_FRONT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Front length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_REAR_LEFT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Rear-left length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_REAR_RIGHT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Rear-right length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_FRONT_RIGHT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Front-right segment start');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_FRONT_LEFT_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Front-left segment length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_SCOOP_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Scoop segment length');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_GRILLE_SEGMENT_LENGTH)).c_str());
    oappend(SET_F("', 'Grille segment legth');"));

    // Repeat for LEFT, RIGHT, and REAR segments...
  }

public:
  void setup()
  {
    p->enable();
    ST->initializePins();
    //Serial.println("All outputs written to PCF8575");
  }

  /**
   * A method that checks if each segment is assigned to the correct LED start and end. If a change/discrepancy is detected, run setup()
   */
  void printSegmentDetails() {}

  /**
   * A method that prints the state of pins 0 - 38 on the ESP32 board
   */
  void printInputPins()
  {
    for (int i = 0; i < 39; i++)
    {
      //Serial.print("Pin ");
      //Serial.print(i);
      //Serial.print(" is ");
      //Serial.println(digitalRead(i));
    }
  }
  void printDetailsPeriodically()
  {
    static unsigned long lastPrintTime = 0;
    const unsigned long printInterval = 3000; // 5 seconds in milliseconds
    if (millis() - lastPrintTime >= printInterval)
    {
      printSegmentDetails();
      // printInputPins();
      lastPrintTime = millis();
    }
  }

  void loop()
  {
    // delay(1000);

    if (!bri || !enabled || !effects.checkSegmentIntegrity() || strip.isUpdating())
    {
      // Print the string "Not running" along with all the values of "bri" and "enabled"
      //  //Serial.print("Not running. bri:");
      //  //Serial.print(bri);
      //  //Serial.print(", enabled:");
      //  //Serial.print(enabled);
      //  //Serial.print(", checkSegmentIntegrity:");
      //  //Serial.print(effects.checkSegmentIntegrity());
      //  //Serial.print(", updating:");
      //  //Serial.println(strip.isUpdating());

      return;
    }
    delay(3000);

    ST->readTelemetry();

    /**
     * Update the state of all things Subaru
     */

    /**
     * Print current state of segment
     */
    printDetailsPeriodically();


  if(ST->driverRockerHigh.isInputActive() || ST->doorOpen.isInputActive()){
    ST->turnOnProjectionRelay();
  }else{
    ST->turnOffProjectionRelay();
  }
    for (int segmentID : ALL_SUBARU_SEGMENT_IDS)
    {


      //Set a RelayState for this segment. Default "false"
      SubaruSegment seg = SegCon::seg(segmentID);
      pendingRelayState[seg.relay] = pendingRelayState[seg.relay] || false;
      
      p->println("-------------------------------------------------", ColorPrint::FG_GREEN, ColorPrint::BG_BLACK);
      p->print("Iterating on segment ", ColorPrint::FG_GREEN, ColorPrint::BG_BLACK);
      p->println(" " + String(segmentID) + " ", ColorPrint::BG_WHITE, ColorPrint::BG_GREEN);
      if(seg.on){
        p->println("Turning on relay for segment " + String(segmentID), ColorPrint::FG_GREEN, ColorPrint::BG_BLACK);
        pendingRelayState[seg.relay] = true;
      }
      // Iterate through effects.allEffects and check to see if this effect should be triggered on this segmentID
      for (auto &effect : effects.allEffects)
      {
        // Check if segmentID exists in the list of effect->segmentIDs
        bool relevantSegment = effect->mySegment(segmentID);
        if (!relevantSegment)
        {
          continue;
        }
        String _modeForSegment = strip.getModeData(seg.mode);
        // p->println("\tMode for segment " + String(segmentID) + " is " + _modeForSegment, ColorPrint::FG_GREEN, ColorPrint::BG_BLACK);
        bool triggering = effect->triggering();
 
        if (triggering)
        {
          p->println("\tQueueing " + effect->name + " on segment " + String(segmentID), ColorPrint::FG_GREEN, ColorPrint::BG_BLACK);
          // If the segmentID exists in the list of effect->segmentIDs, add the effect to the queue
          queueManager.addEffectToQueue(segmentID, effect);
        }
      }
      queueManager.processQueue(segmentID);
    }
    
    //Loop through all pendingRelayStates and update the relay accordingly
    SegCon::getInstance()->checkRelaySegments();
    //printState() every 500ms

    p->println("-------------------------------------------------", ColorPrint::FG_GREEN, ColorPrint::BG_BLACK);

    p->println("");
    p->println("");

  }
};
const char Subaru::_name[] = "Subaru";
const char Subaru::_enabled[] = "enabled";

