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

  void updateSegment(int seg, Effect effect = Effect(), int transition_speed = INSTANT_TRANSITION)
  {
    strip.setTransition(transition_speed);
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

  void triggerGlobalEffect(Effect effect = Effect(FX_MODE_STATIC, 0xFF0000, 255, 128, 0), int transition = SLOW_TRANSITION)
  {

    updateSegment(UNIFIED_SEGMENT, effect, transition);
  }

  void restoreGlobalEffect(int transition_speed = SLOW_TRANSITION)
  {
    updateSegment(UNIFIED_SEGMENT, effects.off, transition_speed);
  }

  void triggerDoorEffect(int transition = SLOW_TRANSITION)
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
     **********************/
    if(ST.brakeEngaged()){
        queueManager.addEffectToQueue(effects.brake, {REAR_SEGMENT}, brake_effect_delay, INSTANT_TRANSITION); // 10 seconds run time, 0.5 seconds transition
    }
    
    /**
     * LEFT EFFECT SEQUENCE
     **********************/
    if(ST.leftIndicatorOn()){
      queueManager.addEffectToQueue(effects.leftTurn, {LEFT_SEGMENT}, left_effect_delay, INSTANT_TRANSITION); // 10 seconds run time, 0.5 seconds transition
    }

    /**
     * RIGHT EFFECT SEQUENCE
     **********************/
    if(ST.rightIndicatorOn()){
      queueManager.addEffectToQueue(effects.rightTurn, {RIGHT_SEGMENT}, right_effect_delay, INSTANT_TRANSITION); // 10 seconds run time, 0.5 seconds transition 
    }

    /**
     * DOOR EFFECT SEQUENCE
     **********************/
    if(ST.doorOpen()){
      queueManager.addEffectToQueue(effects.doorOpen, {LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_SEGMENT}, door_effect_delay, SLOW_TRANSITION);
    }

    /**
     * UNLOCK EFFECT SEQUENCE
     ************************/
    if(ST.unlocked()){
      queueManager.addEffectToQueue(effects.unlock, {LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_SEGMENT}, unlock_effect_delay, INSTANT_TRANSITION); // 10 seconds run time, 0.5 seconds transition
    }

    /**
     * LOCK EFFECT SEQUENCE
     **********************/
    if(ST.locked()){
      queueManager.addEffectToQueue(effects.lock, {LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_SEGMENT}, lock_effect_delay, INSTANT_TRANSITION); // 10 seconds run time, 0.5 seconds transition
    } 

    /**
     * IGNITION EFFECT SEQUENCE
     **************************/
    if(ST.ignitionOn()){
      queueManager.addEffectToQueue(effects.ignition, {LEFT_SEGMENT, RIGHT_SEGMENT, FRONT_SEGMENT, REAR_SEGMENT}, ignition_effect_delay, INSTANT_TRANSITION); // 10 seconds run time, 0.5 seconds transition
    }
    queueManager.processQueue();
  }
};