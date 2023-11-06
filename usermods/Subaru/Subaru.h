#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>
#include "SubaruUtilities.h"

class Subaru : public Usermod
{
private:
  SubaruTelemetry ST = SubaruTelemetry();
  Overrides overrides;
  EffectCacheCollection cache = EffectCacheCollection();
  EffectCollection effects = EffectCollection();

  void updateSegment(int seg, Effect effect)
  {
    // const bool incomingIsPreset = effects.isPreset(effect);
    const bool isTurningOff = effect.checksum == effects.off.checksum;

    if (isTurningOff)
    {
      const auto &previouslySavedEffect = cache.generic.getBySegmentAndChecksum(seg, effect);

      // We only care about non-present effects being restored. Otherwise just turn
      //   off the segment - I don't care anymore.
      //   it's too difficult to try to ressurect a previously running effect on the segment.
      
      if (previouslySavedEffect && !effects.isPreset(previouslySavedEffect))
      {
        effect = *previouslySavedEffect;
      }
    }
    else
    {
      const Effect effectFromSegment = Effect(ST.seg(seg));
      // const bool currentIsPreset = effects.isPreset(effectFromSegment);
      cache.generic.setBySegmentAndChecksum(seg, effect, effectFromSegment);
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

    ST.seg(seg).setOption(SEG_OPTION_ON, on);
    ST.seg(seg).fade_out(fade);
    ST.seg(seg).setColor(0, color1);
    ST.seg(seg).setColor(1, color2);
    ST.seg(seg).setColor(2, color3);
    ST.seg(seg).setPalette(palette);
    ST.seg(seg).speed = speed;

    strip.setMode(seg, mode);
    strip.setBrightness(255, true);
    strip.trigger();
  }
  void restorePreviousState(int segment)
  {
    updateSegment(segment, effects.off);
  }

public:
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

  void triggerGlobalEffect(Effect effect = Effect(FX_MODE_STATIC, 0xFF0000, 255, 128, 0), int transition = 2000)
  {

    strip.setTransition(transition);

    updateSegment(LEFT_SEGMENT, effect);

    updateSegment(RIGHT_SEGMENT, effect);

    updateSegment(BRAKE_SEGMENT, effect);

    updateSegment(FRONT_SEGMENT, effect);
  }

  void restoreGlobalEffect(EffectCache c, int transition = 2000)
  {
    strip.setTransition(transition);
    restorePreviousState(FRONT_SEGMENT);
    // Don't touch the clicker if they're on
    if (!ST.left_indicator_on && !overrides.LeftIndicator)
    {
      restorePreviousState(LEFT_SEGMENT);
    }
    // Don't touch the clicker if they're on
    if (!ST.right_indicator_on && !overrides.RightIndicator)
    {
      restorePreviousState(RIGHT_SEGMENT);
    }
    // Don't touch the brake if it's being pressed
    if (!ST.brake_pedal_pressed && !overrides.Brake && !ST.car_in_reverse && !overrides.Reverse)
    {
      strip.setTransition(0);
      restorePreviousState(BRAKE_SEGMENT);
    }
  }

  void triggerDoorEffect(int transition = 2000)
  {
    triggerGlobalEffect(effects.doorOpen, transition);
    Serial.println("++++++ DOOR EFFECT ACTIVATED ++++++");
  }

  void triggerUnlockEffect()
  {
    triggerGlobalEffect(effects.unlock);
    Serial.println("++++++ UNLOCK EFFECT ACTIVATED ++++++");
  }

  void triggerLockEffect()
  {
    triggerGlobalEffect(effects.lock);
    Serial.println("++++++ LOCK EFFECT ACTIVATED ++++++");
  }

  void triggerIgnitionEffect()
  {
    triggerGlobalEffect(effects.ignition);
    Serial.println("++++++ IGNITION EFFECT ACTIVATED ++++++");
  }

  void loop()
  {
    if (!ST.checkSegmentIntegrity() || strip.isUpdating())
      return;
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
    const bool reverse_state_change = reverse_previous_state != ST.car_in_reverse;
    reverse_previous_state = ST.car_in_reverse;

    const bool brake_state_change = brake_pedal_previous_state != ST.brake_pedal_pressed;
    brake_pedal_previous_state = ST.brake_pedal_pressed;

    overrides.setBrake(last_brake_press_time && (millis() - last_brake_press_time < brake_effect_delay));
    if (reverse_state_change || brake_state_change || brake_previously_set)
    {
      if (ST.brake_pedal_pressed || ST.car_in_reverse)
      {
        if (!brake_previously_set || reverse_state_change)
        {
          if (!overrides.checkOthers("brake"))
          {
            cache.forBrake.refresh();
            strip.setTransition(0);
          }
          if (ST.car_in_reverse)
          {
            updateSegment(BRAKE_SEGMENT, effects.reverse);
            Serial.println("++++++ REVERSE PRESSED ++++++");
          }
          else
          {
            updateSegment(BRAKE_SEGMENT, effects.brake);
            Serial.println("++++++ BRAKE PRESSED ++++++");
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
          strip.setTransition(0);
          cache.forDoor.setRear(cache.forBrake.RearSegment);
          // Enable door mode.
          updateSegment(BRAKE_SEGMENT, effects.doorOpen);
        }
        else
        {
          strip.setTransition(0);
          restorePreviousState(BRAKE_SEGMENT);
        }
        Serial.println("------ BRAKE/REVERSE RELEASED ------");
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
          strip.setTransition(0);
          if (!overrides.Door)
          {
            cache.forLeftTurn.refresh();
          }

          updateSegment(LEFT_SEGMENT, effects.leftTurn);
          Serial.println("++++++ LEFT CLICKER ON ++++++");
        }
        left_previously_set = true;
        last_left_on_time = millis();
      }
      else if (!overrides.LeftIndicator)
      {
        // Don't reset fully if the door is open.
        strip.setTransition(1000);
        if (!overrides.Door)
        {
          restorePreviousState(LEFT_SEGMENT);
        }
        else
        {
          // Pass the clicker cache to door cache
          // cache.forDoor.setLeft(cache.forLeftTurn.LeftSegment);
          // Enable door mode.
          triggerDoorEffect(INSTANT_TRANSITION);
        }
        Serial.println("------ LEFT CLICKER OFF ------");
        last_left_on_time = 0;
        left_previously_set = false;
      }
    }
    else if (!ST.left_indicator_on && !overrides.LeftIndicator)
    {
      // Stop caching state when door is open.
      if (!ST.door_is_open && !!overrides.Door)
      {
        cache.forLeftTurn.setLeft();
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
        if (!right_previously_set)
        {
          strip.setTransition(0);
          if (!overrides.Door)
          {
            cache.forRightTurn.refresh();
          }
          updateSegment(RIGHT_SEGMENT, effects.rightTurn);
          Serial.println("++++++ RIGHT TURN EFFECT ACTIVATED ++++++");
        }

        right_previously_set = true;
        last_right_on_time = millis();
      }
      else if (!overrides.RightIndicator)
      {
        // Don't reset fully if the door is open.
        strip.setTransition(1000);

        if (!overrides.Door)
        {
          restorePreviousState(RIGHT_SEGMENT);
        }
        else
        {
          // Pass the clicker cache to door cache
          // cache.forDoor.setRight(cache.forRightTurn.RightSegment);
          // Enable door mode.
          triggerDoorEffect(INSTANT_TRANSITION);
        }
        Serial.println("------ RIGHT TURN EFFECT DEACTIVATED ------");

        right_previously_set = false;
        last_right_on_time = 0;
      }
    }
    else if (!ST.right_indicator_on && !overrides.RightIndicator)
    {
      // Stop caching state when door is open.
      if (!ST.door_is_open && !overrides.Door)
      {
        cache.forRightTurn.setRight();
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
        if (!door_previously_set)
        {
          triggerDoorEffect();
        }
        door_previously_set = true;
        last_door_open_time = millis();
      }
      else if (!overrides.Door)
      {
        restoreGlobalEffect(cache.forDoor);
        Serial.println("------ DOOR EFFECT DEACTIVATED ------");

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
      if (ST.doors_unlocked && !unlockOverrideExpired && !doors_unlocked_previously_set)
      {

        triggerGlobalEffect(effects.unlock, INSTANT_TRANSITION);
        Serial.println("++++++ UNLOCK EFFECT ACTIVATED ++++++");
        doors_unlocked_previously_set = true;
        last_unlock_time = millis();
      }
      else if (!overrides.DoorsUnlocked)
      {
        if (!overrides.Door)
        {
          restoreGlobalEffect(cache.forUnlock, INSTANT_TRANSITION);
        }
        else
        {
          triggerDoorEffect(INSTANT_TRANSITION);
        }
        Serial.println("------ UNLOCK EFFECT DEACTIVATED ------");
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
      if (ST.doors_locked && !lockOverrideExpired && !doors_locked_previously_set)
      {
        triggerGlobalEffect(effects.lock, INSTANT_TRANSITION);
        Serial.println("++++++ LOCK EFFECT ACTIVATED ++++++");
        doors_locked_previously_set = true;
        last_lock_time = millis();
      }
      else if (!overrides.DoorsLocked)
      {
        if (!overrides.Door)
        {
          restoreGlobalEffect(cache.forLock, INSTANT_TRANSITION);
        }
        else
        {
          triggerDoorEffect(INSTANT_TRANSITION);
        }
        Serial.println("------ LOCK EFFECT DEACTIVATED ------");
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
      if (ST.ignition && !ignitionOverrideExpired && !ignition_previously_set)
      {
        triggerGlobalEffect(effects.ignition, INSTANT_TRANSITION);
        Serial.println("++++++ IGNITION EFFECT ACTIVATED ++++++");
        ignition_previously_set = true;
        last_ignition_time = millis();
      }
      else if (!overrides.Ignition)
      {
        if (!overrides.Door)
        {
          restoreGlobalEffect(cache.forIgnition, INSTANT_TRANSITION);
        }
        else
        {
          triggerDoorEffect(INSTANT_TRANSITION);
        }
        Serial.println("------ IGNITION EFFECT DEACTIVATED ------");
        ignition_previously_set = false;
        last_ignition_time = 0;
      }
    }
    else if (!ST.door_is_open && !overrides.Door)
    {
    }
  }
};