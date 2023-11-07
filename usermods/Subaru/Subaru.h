#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>
#include "SubaruUtilities.h"

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
  Overrides overrides;
  Effect effect;
  EffectCollection effects = EffectCollection();
  EffectCacheCollection cache = EffectCacheCollection();

  SubaruTelemetry ST;
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
  void transition(SubaruTransition transition)
  {
    // Check if transition.effect is defined, if not set effect to effects.off
    Effect effect = transition.effect;
    Effect effectStarter = transition.effectStarter;
    int segmentId = transition.segment;

    strip.setTransition(transition.speed);

    std::array<int, 4> segmentIds = {LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_SEGMENT}; // Assuming unified segments are always these IDs.
    std::vector<int> segmentsToUpdate;

    if (transition.segment == UNIFIED_SEGMENT)
    {
      segmentsToUpdate.assign(segmentIds.begin(), segmentIds.end());
    }
    else
    {
      segmentsToUpdate.push_back(transition.segment);
    }

    for (int segmentId : segmentsToUpdate)
    {
      ST.selectAndEnable(segmentId);

      const bool isTurningOff = effect.checksum == effects.off.checksum;
      const Effect effectFromSegment = Effect(ST.seg(segmentId));

      if (isTurningOff)
      {
        gray("Turning off segment " + String(segmentId));
        const auto &previouslySavedEffect = cache.generic.getBySegmentAndChecksum(segmentId, effectStarter);
        if (previouslySavedEffect)
        {
          effect = *previouslySavedEffect;
        }
      }
      else
      {
        gray("Setting segment " + String(segmentId) + " to effect " + String(effect.checksum));
        gray("Storing effect " + String(effectFromSegment.checksum) + " in cache");
        cache.generic.setBySegmentAndChecksum(segmentId, effectStarter, effectFromSegment);
      }
      int mode = effect.mode;
      uint32_t color1 = effect.colors[0];
      uint32_t color2 = effect.colors[1];
      uint32_t color3 = effect.colors[2];
      int speed = effect.speed;
      uint8_t fade = effect.fade;
      uint8_t palette = effect.palette;
      bool on = effect.power;

      if (!on)
      {
        color1 = 0x000000;
        color2 = 0x000000;
        color3 = 0x000000;
      }

      ST.seg(segmentId).setOption(SEG_OPTION_ON, on);
      ST.seg(segmentId).fade_out(fade);
      ST.seg(segmentId).setColor(0, color1);
      ST.seg(segmentId).setColor(1, color2);
      ST.seg(segmentId).setColor(2, color3);
      ST.seg(segmentId).setPalette(palette);
      ST.seg(segmentId).speed = speed;

      strip.setMode(segmentId, mode);
    }
  }
  void updateSegment(int seg, Effect effect = Effect(), int transition_speed = INSTANT_TRANSITION)
  {
    strip.setTransition(transition_speed);

    std::array<int, 4> segmentIds = {LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_SEGMENT}; // Assuming unified segments are always these IDs.
    std::vector<int> segmentsToUpdate;

    if (seg == UNIFIED_SEGMENT)
    {
      segmentsToUpdate.assign(segmentIds.begin(), segmentIds.end());
    }
    else
    {
      segmentsToUpdate.push_back(seg);
    }

    for (int segmentId : segmentsToUpdate)
    {
      ST.selectAndEnable(segmentId);

      const bool isTurningOff = effect.checksum == effects.off.checksum;

      if (isTurningOff)
      {
        gray("Turning off segment " + String(segmentId));
        const auto &previouslySavedEffect = cache.generic.getBySegmentAndChecksum(segmentId, effect);

        if (previouslySavedEffect && !effects.isPreset(previouslySavedEffect))
        {
          effect = *previouslySavedEffect;
        }
      }
      else
      {
        gray("Setting segment " + String(segmentId) + " to effect " + String(effect.checksum));
        const Effect effectFromSegment = Effect(ST.seg(segmentId));
        gray("Storing effect " + String(effectFromSegment.checksum) + " in cache");

        cache.generic.setBySegmentAndChecksum(segmentId, effect, effectFromSegment);
      }
      int mode = effect.mode;
      uint32_t color1 = effect.colors[0];
      uint32_t color2 = effect.colors[1];
      uint32_t color3 = effect.colors[2];
      int speed = effect.speed;
      uint8_t fade = effect.fade;
      uint8_t palette = effect.palette;
      bool on = effect.power;

      if (!on)
      {
        color1 = 0x000000;
        color2 = 0x000000;
        color3 = 0x000000;
      }

      ST.seg(segmentId).setOption(SEG_OPTION_ON, on);
      ST.seg(segmentId).fade_out(fade);
      ST.seg(segmentId).setColor(0, color1);
      ST.seg(segmentId).setColor(1, color2);
      ST.seg(segmentId).setColor(2, color3);
      ST.seg(segmentId).setPalette(palette);
      ST.seg(segmentId).speed = speed;

      strip.setMode(segmentId, mode);
    }

    strip.setBrightness(255, true);
    strip.trigger();
  }
  void restorePreviousState(int segment)
  {
    updateSegment(segment, effects.off);
  }

public:
  void white(String message)
  {
    Serial.print("\033[37m");
    Serial.println(message);
  }
  void red(String message)
  {
    Serial.print("\033[31m");
    Serial.println(message);
  }
  void green(String message)
  {
    Serial.print("\033[32m");
    Serial.println(message);
  }
  void yellow(String message)
  {
    Serial.print("\033[33m");
    Serial.println(message);
  }
  void blue(String message)
  {
    Serial.print("\033[34m");
    Serial.println(message);
  }
  void magenta(String message)
  {
    Serial.print("\033[35m");
    Serial.println(message);
  }
  void cyan(String message)
  {
    Serial.print("\033[36m");
    Serial.println(message);
  }
  void orange(String message)
  {
    Serial.print("\033[38;5;208m");
    Serial.println(message);
  }
  void gray(String message)
  {
    Serial.print("\033[38;5;240m");
    Serial.println(message);
  }
  /**
   * Function to print to console with yellow text with a green highlight
   * @param message The message to print
   */
  void printYellowGreen(String message)
  {
    Serial.print("\033[38;5;226m");
    Serial.print("\033[48;5;28m");
    Serial.println(message);
  }
  void setup()
  {
    ST.setupSegments();
    Wire.begin(SubaruTelemetry::SDA_PIN, SubaruTelemetry::SCL_PIN);
    writePCF8575(0x0000); // Configure all pins as outputs initially
    Serial.println("All outputs written to PCF8575");
  }
  /**
   * A method that checks if each segment is assigned to the correct LED start and end. If a change/discrepancy is detected, run setup()
   */

  void printSegmentDetails() {
    gray("FRONT: " + Effect(ST.seg(FRONT_SEGMENT)).checksum);
    gray("LEFT" + Effect(ST.seg(FRONT_SEGMENT)).checksum);
    gray("RIGHT: " + Effect(ST.seg(FRONT_SEGMENT)).checksum);
    gray("REAR: " + Effect(ST.seg(FRONT_SEGMENT)).checksum);
  }

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

  void printGateKeepers()
  {
    // Print all conditional boolean values to serial monitor
    Serial.print("enabled: ");
    Serial.println(enabled);
    Serial.print("ST.checkSegmentIntegrity(): ");
    Serial.println(ST.checkSegmentIntegrity());
    Serial.print("strip.isUpdating(): ");
    Serial.println(strip.isUpdating());

    Serial.println("Subaru is disabled or segment integrity is compromised. Skipping loop.");
  }
  void loop()
  {
    /***************************
    ****************************
     * 
     *  LET THE LOOP BEGIN
     * 
    ****************************
    ****************************/

    
    if (!enabled || !ST.checkSegmentIntegrity() || strip.isUpdating())
    {
      // return printGateKeepers();
      return;
    }
    static bool left_previously_set = false;
    static bool door_previously_set = false;
    static bool right_previously_set = false;
    static bool brake_previously_set = false;
    static bool doors_unlocked_previously_set = false;
    static bool doors_locked_previously_set = false;
    static bool ignition_previously_set = false;

    static bool brake_pedal_previous_state = false;
    static bool reverse_previous_state = false;
    static bool left_indicator_previous_state = false;
    static bool right_indicator_previous_state = false;
    static bool door_previous_state = false;
    static bool doors_unlocked_previous_state = false;
    static bool doors_locked_previous_state = false;
    static bool ignition_previous_state = false;

    // Declare the struct for PreviousState

    // static unsigned long brake_effect_expiration_timer = 0;
    // static unsigned long left_effect_expiration_timer = 0;
    // static unsigned long right_effect_expiration_timer = 0;
    // static unsigned long door_effect_expiration_timer = 0;
    // static unsigned long unlock_effect_expiration_timer = 0;
    // static unsigned long lock_effect_expiration_timer = 0;
    // static unsigned long ignition_effect_expiration_timer = 0;

    static unsigned long last_brake_press_time = 0;
    static unsigned long last_left_on_time = 0;
    static unsigned long last_right_on_time = 0;
    static unsigned long last_door_open_time = 0;
    static unsigned long last_unlock_time = 0;
    static unsigned long last_lock_time = 0;
    static unsigned long last_ignition_time = 0;

    static unsigned long brake_effect_delay = 0;
    static unsigned long left_effect_delay = 3000;
    static unsigned long right_effect_delay = 3000;
    static unsigned long door_effect_delay = 10000;
    static unsigned long unlock_effect_delay = 3000;
    static unsigned long lock_effect_delay = 3000;
    static unsigned long ignition_effect_delay = 3000;
    /**
     * Update the state of all things Subaru
     */

    ST.readTelemetry();

    /**
     * Print current state of segment
     */
    printDetailsPeriodically();

    /**
     * BRAKE EFFECT SEQUENCE
     ***************************/

    const bool reverse_state_change = reverse_previous_state != ST.car_in_reverse;
    reverse_previous_state = ST.car_in_reverse;

    const bool brake_state_change = brake_pedal_previous_state != ST.brake_pedal_pressed;
    brake_pedal_previous_state = ST.brake_pedal_pressed;

    overrides.setBrake(last_brake_press_time && (millis() - last_brake_press_time < brake_effect_delay));
    if (reverse_state_change || brake_state_change || brake_previously_set)
    {
      if (ST.brake_pedal_pressed || ST.car_in_reverse)
      {
        queueManager.addToQueue(effects.brake, {REAR_SEGMENT}, std::chrono::seconds(brake_effect_delay), INSTANT_TRANSITION);

        if (!brake_previously_set || reverse_state_change)
        {
          if (ST.car_in_reverse)
          {
            red("++++++ REVERSE PRESSED ++++++");
          }
          else
          {
            //updateSegment(REAR_SEGMENT, effects.brake);
            red("++++++ BRAKE PRESSED ++++++");
          }
        }
        brake_previously_set = true;
        last_brake_press_time = millis();
      }
      else if (!overrides.Brake)
      {
        // strip.setTransition(1000);
        // Don't reset fully if the door is open.
        if (overrides.Door)
        {
          //strip.setTransition(0);
          //cache.forDoor.setRear(cache.forBrake.RearSegment);
          // Enable door mode.
          //updateSegment(REAR_SEGMENT, effects.doorOpen);
        }
        else
        {
          strip.setTransition(0);
          //updateSegment(REAR_SEGMENT);
        }
        red("------ BRAKE/REVERSE RELEASED ------");
        brake_previously_set = false;
        last_brake_press_time = 0;
      }
    }
    else if (!(ST.brake_pedal_pressed || ST.car_in_reverse) && !overrides.Brake)
    {
    }

    /**
     * LEFT TURN EFFECT SEQUENCE
     ****************************/

    const bool left_state_change = left_indicator_previous_state != ST.left_indicator_on;
    left_indicator_previous_state = ST.left_indicator_on;

    overrides.setLeftIndicator(last_left_on_time && (millis() - last_left_on_time < left_effect_delay));

    if (left_state_change || left_previously_set)
    {
      if (ST.left_indicator_on)
      {
        if (!left_previously_set)
        {
          queueManager.addToQueue(effects.leftTurn, {LEFT_SEGMENT}, std::chrono::seconds(left_effect_delay), SLOW_TRANSITION);

          //strip.setTransition(0);
          if (!overrides.Door)
          {
            //cache.forLeftTurn.refresh();
          }

          //updateSegment(LEFT_SEGMENT, effects.leftTurn);
          orange("------ LEFT TURN EFFECT ACTIVATED ------");
        }
        left_previously_set = true;
        last_left_on_time = millis();
      }
      else if (!overrides.LeftIndicator)
      {
        // Don't reset fully if the door is open.
        //strip.setTransition(1000);
        if (!overrides.Door)
        {
          //updateSegment(LEFT_SEGMENT);
        }
        else
        {
          // Pass the clicker cache to door cache
          // cache.forDoor.setLeft(cache.forLeftTurn.LeftSegment);
          // Enable door mode.
          //triggerDoorEffect(INSTANT_TRANSITION);
        }
        orange("------ LEFT TURN EFFECT DEACTIVATED ------");
        last_left_on_time = 0;
        left_previously_set = false;
      }
    }
    else if (!ST.left_indicator_on && !overrides.LeftIndicator)
    {
      // Stop caching state when door is open.
      if (!ST.door_is_open && !!overrides.Door)
      {
        //cache.forLeftTurn.setLeft();
      }
    }

    /**
     * RIGHT CLICKER SEQUENCE
     ***************************/

    const bool right_state_change = right_indicator_previous_state != ST.right_indicator_on;
    right_indicator_previous_state = ST.right_indicator_on;
    overrides.setRightIndicator(last_right_on_time && (millis() - last_right_on_time < right_effect_delay));
    if (right_state_change || right_previously_set)
    {
      if (ST.right_indicator_on)
      {
        queueManager.addToQueue(effects.leftTurn, {RIGHT_SEGMENT}, std::chrono::seconds(right_effect_delay), SLOW_TRANSITION);

        if (!right_previously_set)
        {
          //updateSegment(RIGHT_SEGMENT, effects.rightTurn, 0);
          orange("------ RIGHT TURN EFFECT ACTIVATED ------");
        }

        right_previously_set = true;
        last_right_on_time = millis();
      }
      else if (!overrides.RightIndicator)
      {
        // Don't reset fully if the door is open.
        //strip.setTransition(1000);

        if (!overrides.Door)
        {
          //updateSegment(RIGHT_SEGMENT);
        }
        else
        {
          // Pass the clicker cache to door cache
          // cache.forDoor.setRight(cache.forRightTurn.RightSegment);
          // Enable door mode.
          //triggerDoorEffect(INSTANT_TRANSITION);
        }
        orange("------ RIGHT TURN EFFECT DEACTIVATED ------");

        right_previously_set = false;
        last_right_on_time = 0;
      }
    }
    else if (!ST.right_indicator_on && !overrides.RightIndicator)
    {
      // Stop caching state when door is open.
      if (!ST.door_is_open && !overrides.Door)
      {
        //cache.forRightTurn.setRight();
      }
    }

    /**
     * DOOR OPEN EFFECT SEQUENCE
     ***************************/

    const bool door_state_change = door_previous_state != ST.door_is_open;
    door_previous_state = ST.door_is_open;

    overrides.setDoor(last_door_open_time && (millis() - last_door_open_time < door_effect_delay));
    if (door_state_change || door_previously_set)
    {
      if (ST.door_is_open)
      {
        queueManager.addToQueue(effects.doorOpen, {FRONT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT, LEFT_SEGMENT}, std::chrono::seconds(door_effect_delay), SLOW_TRANSITION);
        green("++++++ DOOR EFFECT ACTIVATED ++++++");

        if (!door_previously_set)
        {
          //triggerDoorEffect();
        }
        door_previously_set = true;
        last_door_open_time = millis();
      }
      else if (!overrides.Door)
      {
        //restoreGlobalEffect();
        green("------ DOOR EFFECT DEACTIVATED ------");

        door_previously_set = false;
        last_door_open_time = 0;
      }
    }
    else if (!ST.door_is_open && !overrides.Door)
    {
    }

    /**
     * DOORS UNLOCKED SEQUENCE
     ***************************/

    const bool doors_unlocked_state_change = doors_unlocked_previous_state != ST.doors_unlocked;
    doors_unlocked_previous_state = ST.doors_unlocked;
    overrides.setDoorsUnlocked(last_unlock_time && (millis() - last_unlock_time < unlock_effect_delay));
    const bool unlockOverrideExpired = !doors_unlocked_state_change && !overrides.DoorsUnlocked && ST.doors_unlocked;
    if (doors_unlocked_state_change || doors_unlocked_previously_set)
    {
      if (ST.doors_unlocked)
      {
        
        queueManager.addToQueue(effects.unlock, {FRONT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT, LEFT_SEGMENT}, std::chrono::seconds(unlock_effect_delay), SLOW_TRANSITION);
            green("++++++ UNLOCK EFFECT ACTIVATED ++++++");

        //triggerUnlockEffect();
        doors_unlocked_previously_set = true;
        last_unlock_time = millis();
      }
      else if (!overrides.DoorsUnlocked)
      {
        if (!overrides.Door)
        {
          //restoreGlobalEffect(INSTANT_TRANSITION);
        }
        else
        {
          //triggerDoorEffect(INSTANT_TRANSITION);
        }
        green("------ UNLOCK EFFECT DEACTIVATED ------");
        doors_unlocked_previously_set = false;
        last_unlock_time = 0;
      }
    }
    else if (!ST.door_is_open && !overrides.Door)
    {
    }

    /**
     * DOORS LOCKED SEQUENCE
     ***************************/

    const bool doors_locked_state_change = doors_locked_previous_state != ST.doors_locked;
    doors_locked_previous_state = ST.doors_locked;
    overrides.setDoorsLocked(last_lock_time && (millis() - last_lock_time < lock_effect_delay));
    const bool lockOverrideExpired = !doors_locked_state_change && !overrides.DoorsLocked && ST.doors_locked;
    if (doors_locked_state_change || doors_locked_previously_set)
    {
      if (ST.doors_locked)
      {
        queueManager.addToQueue(effects.lock, {FRONT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT, LEFT_SEGMENT}, std::chrono::seconds(lock_effect_delay), SLOW_TRANSITION);
        green("++++++ LOCK EFFECT ACTIVATED ++++++");

        //triggerLockEffect();
        doors_locked_previously_set = true;
        last_lock_time = millis();
      }
      else if (!overrides.DoorsLocked)
      {
        if (!overrides.Door)
        {
          //restoreGlobalEffect(INSTANT_TRANSITION);
        }
        else
        {
          //triggerDoorEffect(INSTANT_TRANSITION);
        }
        red("------ LOCK EFFECT DEACTIVATED ------");
        doors_locked_previously_set = false;
        last_lock_time = 0;
      }
    }
    else if (!ST.door_is_open && !overrides.Door)
    {
    }

    /**
     * IGNITION EFFECT SEQUENCE
     **************************/

    const bool ignition_state_change = ignition_previous_state != ST.ignition;
    ignition_previous_state = ST.ignition;
    overrides.setIgnition(last_ignition_time && (millis() - last_ignition_time < ignition_effect_delay));
    const bool ignitionOverrideExpired = !ignition_state_change && !overrides.Ignition && ST.ignition;
    if (ignition_state_change || ignition_previously_set)
    {
      if (ST.ignition)
      {
        queueManager.addToQueue(effects.ignition, {FRONT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT, LEFT_SEGMENT}, std::chrono::seconds(ignition_effect_delay), INSTANT_TRANSITION);
        //triggerIgnitionEffect();
        ignition_previously_set = true;
        last_ignition_time = millis();
      }
      else if (!overrides.Ignition)
      {
        if (!overrides.Door)
        {
          //restoreGlobalEffect(INSTANT_TRANSITION);
        }
        else
        {
          //triggerDoorEffect(INSTANT_TRANSITION);
        }
        yellow("------ IGNITION EFFECT DEACTIVATED ------");
        ignition_previously_set = false;
        last_ignition_time = 0;
      }
    }
    else if (!ST.door_is_open && !overrides.Door)
    {
    }

    queueManager.processQueue();

  }
};
const char Subaru::_name[] = "Subaru";
const char Subaru::_enabled[] = "enabled";