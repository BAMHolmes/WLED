#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>
#include "classes/QueueManager.h"

const char UM_FRONT_SEGMENT_START[] PROGMEM = "frontSegmentStart";
const char UM_FRONT_SEGMENT_END[] PROGMEM = "frontSegmentEnd";
const char UM_LEFT_SEGMENT_START[] PROGMEM = "leftSegmentStart";
const char UM_LEFT_SEGMENT_END[] PROGMEM = "leftSegmentEnd";
const char UM_RIGHT_SEGMENT_START[] PROGMEM = "rightSegmentStart";
const char UM_RIGHT_SEGMENT_END[] PROGMEM = "rightSegmentEnd";
const char UM_REAR_SEGMENT_START[] PROGMEM = "rearSegmentStart";
const char UM_REAR_SEGMENT_END[] PROGMEM = "rearSegmentEnd";
const char UM_UNIFIED_SEGMENT_START[] PROGMEM = "unifiedSegmentStart";
const char UM_UNIFIED_SEGMENT_END[] PROGMEM = "unifiedSegmentEnd";

class Subaru : public Usermod

{
private:
  QueueManager queueManager;

  // Usermod variable declarations for storing segment indices

  static const char _name[];
  static const char _enabled[];
  bool enabled = false;

  void addToConfig(JsonObject &root) override
  {
    JsonObject top = root.createNestedObject(FPSTR(_name));
    top[FPSTR(_enabled)] = enabled;

    top[FPSTR(UM_FRONT_SEGMENT_START)] = SUBARU_SEGMENT_CONFIG.frontStart;
    top[FPSTR(UM_FRONT_SEGMENT_END)] = SUBARU_SEGMENT_CONFIG.frontEnd;
    top[FPSTR(UM_LEFT_SEGMENT_START)] = SUBARU_SEGMENT_CONFIG.leftStart;
    top[FPSTR(UM_LEFT_SEGMENT_END)] = SUBARU_SEGMENT_CONFIG.leftEnd;
    top[FPSTR(UM_RIGHT_SEGMENT_START)] = SUBARU_SEGMENT_CONFIG.rightStart;
    top[FPSTR(UM_RIGHT_SEGMENT_END)] = SUBARU_SEGMENT_CONFIG.rightEnd;
    top[FPSTR(UM_REAR_SEGMENT_START)] = SUBARU_SEGMENT_CONFIG.rearStart;
    top[FPSTR(UM_REAR_SEGMENT_END)] = SUBARU_SEGMENT_CONFIG.rearEnd;
    top[FPSTR(UM_UNIFIED_SEGMENT_START)] = SUBARU_SEGMENT_CONFIG.unifiedStart;
    top[FPSTR(UM_UNIFIED_SEGMENT_END)] = SUBARU_SEGMENT_CONFIG.unifiedEnd;
  }

  bool readFromConfig(JsonObject &root) override
  {
    JsonObject top = root[FPSTR(_name)];
    bool configComplete = !top.isNull();

    configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
    configComplete &= getJsonValue(top[FPSTR(UM_FRONT_SEGMENT_START)], SUBARU_SEGMENT_CONFIG.frontStart);
    configComplete &= getJsonValue(top[FPSTR(UM_FRONT_SEGMENT_END)], SUBARU_SEGMENT_CONFIG.frontEnd);
    configComplete &= getJsonValue(top[FPSTR(UM_LEFT_SEGMENT_START)], SUBARU_SEGMENT_CONFIG.leftStart);
    configComplete &= getJsonValue(top[FPSTR(UM_LEFT_SEGMENT_END)], SUBARU_SEGMENT_CONFIG.leftEnd);
    configComplete &= getJsonValue(top[FPSTR(UM_RIGHT_SEGMENT_START)], SUBARU_SEGMENT_CONFIG.rightStart);
    configComplete &= getJsonValue(top[FPSTR(UM_RIGHT_SEGMENT_END)], SUBARU_SEGMENT_CONFIG.rightEnd);
    configComplete &= getJsonValue(top[FPSTR(UM_REAR_SEGMENT_START)], SUBARU_SEGMENT_CONFIG.rearStart);
    configComplete &= getJsonValue(top[FPSTR(UM_REAR_SEGMENT_END)], SUBARU_SEGMENT_CONFIG.rearEnd);
    configComplete &= getJsonValue(top[FPSTR(UM_UNIFIED_SEGMENT_START)], SUBARU_SEGMENT_CONFIG.unifiedStart);
    configComplete &= getJsonValue(top[FPSTR(UM_UNIFIED_SEGMENT_END)], SUBARU_SEGMENT_CONFIG.unifiedEnd);

    if (configComplete)
    {
      ST = SubaruTelemetry();
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
    oappend(String(FPSTR(UM_FRONT_SEGMENT_START)).c_str());
    oappend(SET_F("', 'Front segment start');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_FRONT_SEGMENT_START)).c_str());
    oappend(SET_F("', 'Front segment end');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_LEFT_SEGMENT_START)).c_str());
    oappend(SET_F("', 'Left segment start');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_LEFT_SEGMENT_END)).c_str());
    oappend(SET_F("', 'Left segment end');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_RIGHT_SEGMENT_START)).c_str());
    oappend(SET_F("', 'Right segment start');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_RIGHT_SEGMENT_END)).c_str());
    oappend(SET_F("', 'Right segment end');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_REAR_SEGMENT_START)).c_str());
    oappend(SET_F("', 'Rear segment start');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_REAR_SEGMENT_END)).c_str());
    oappend(SET_F("', 'Rear segment end');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_UNIFIED_SEGMENT_START)).c_str());
    oappend(SET_F("', 'Unified segment start');"));
    oappend(SET_F("addTextInput('"));
    oappend(configName.c_str());
    oappend(String(FPSTR(UM_UNIFIED_SEGMENT_END)).c_str());
    oappend(SET_F("', 'Unified segment end');"));

    // Repeat for LEFT, RIGHT, and REAR segments...
  }


public:
  void setup()
  {
    effects.off.start();
    Wire.begin(SubaruTelemetry::SDA_PIN, SubaruTelemetry::SCL_PIN);
    writePCF8575(0x0000); // Configure all pins as outputs initially
    Serial.println("All outputs written to PCF8575");
  }
  /**
   * A method that checks if each segment is assigned to the correct LED start and end. If a change/discrepancy is detected, run setup()
   */

  void printSegmentDetails() {}

  void printDetailsPeriodically()
  {
    static unsigned long lastPrintTime = 0;
    const unsigned long printInterval = 5000; // 5 seconds in milliseconds
    if (millis() - lastPrintTime >= printInterval)
    {
      printSegmentDetails();
      lastPrintTime = millis();
    }
  }
  void writePCF8575(uint16_t data)
  {
    Wire.beginTransmission(SubaruTelemetry::PCF8575_ADDRESS); // Begin transmission to PCF8575
    Wire.write(data & 0xFF);                                  // Write low byte
    Wire.write(data >> 8);                                    // Write high byte
    Wire.endTransmission();                                   // End transmission
  }



  void loop()
  {
    
    if (!bri || !enabled || !effects.checkSegmentIntegrity() || strip.isUpdating())
      return;

    /**
     * Update the state of all things Subaru
     */



    /**
     * Print current state of segment
     */
    printDetailsPeriodically();


    /**
     * BRAKE EFFECT SEQUENCE
     **********************/
    if(ST.brakeEngaged()){
        queueManager.addEffectToQueue(effects.brake); // 10 seconds run time, 0.5 seconds transition
    }

    /**
     * REVERSE EFFECT SEQUENCE
     * **********************/
    if(ST.reverseEngaged()){
      queueManager.addEffectToQueue(effects.reverse); // 10 seconds run time, 0.5 seconds transition
    }

    /**
     * LEFT EFFECT SEQUENCE
     **********************/
    if(ST.leftIndicatorOn()){
      queueManager.addEffectToQueue(effects.leftTurn); // 10 seconds run time, 0.5 seconds transition
    }

    /**
     * RIGHT EFFECT SEQUENCE
     **********************/
    if(ST.rightIndicatorOn()){
      queueManager.addEffectToQueue(effects.rightTurn); // 10 seconds run time, 0.5 seconds transition 
    }

    /**
     * DOOR EFFECT SEQUENCE
     **********************/
    if(ST.doorOpen()){
      queueManager.addEffectToQueue(effects.doorOpen);
    }

    /**
     * UNLOCK EFFECT SEQUENCE
     ************************/
    if(ST.unlocked()){
      queueManager.addEffectToQueue(effects.unlock); // 10 seconds run time, 0.5 seconds transition
    }

    /**
     * LOCK EFFECT SEQUENCE
     **********************/
    if(ST.locked()){
      queueManager.addEffectToQueue(effects.lock); // 10 seconds run time, 0.5 seconds transition
    } 

    /**
     * IGNITION EFFECT SEQUENCE
     **************************/
    if(ST.ignitionOn()){
      queueManager.addEffectToQueue(effects.ignition); // 10 seconds run time, 0.5 seconds transition
    }

    queueManager.processQueue();
    
    ST.readTelemetry();
  }
};
const char Subaru::_name[] = "Subaru";
const char Subaru::_enabled[] = "enabled";
