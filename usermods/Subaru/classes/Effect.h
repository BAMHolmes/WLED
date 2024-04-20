#ifndef EFFECT_H
#define EFFECT_H

#include "ColorPrint.h"
#include "SubaruTelemetry.h"

#include "SegCon.h"
#include <functional>
enum SwitchType
{
    MOMENTARY,
    TOGGLE
};

/**
 * @brief Class for managing the effect for each segment.
 */
class Effect
{
public:
    //using TriggerCondition = bool (PinState::*)() const;

    String name;
    std::function<bool()> triggerCondition;
    std::vector<int> segmentIDs;
    uint8_t mode;
    bool reverse;
    int transitionSpeed;
    int interimTransitionSpeed;
    uint32_t colors[3];
    uint8_t speed;
    uint8_t fade;
    bool power;
    unsigned long startTime;
    bool isRunning;
    unsigned long runTime;
    unsigned long remainingRuntime;
    uint8_t palette;
    uint8_t intensity;
    String checksum;
    Effect(String n)
        : name(n),
          triggerCondition([&]() -> bool { return true; }),
          segmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS),
          mode(FX_MODE_STATIC),
          transitionSpeed(INSTANT_TRANSITION),
          colors{0x000000, 0x000000, 0x000000},
          speed(255),
          fade(255),
          power(true),
          startTime(0),
          isRunning(false),
          runTime(0),
          remainingRuntime(0),
          palette(0),
          intensity(255),
          checksum("") // Ensure checksum is calculated last
    {
        reverse = false;
        interimTransitionSpeed = -1;
        checksum = calculateChecksum();
    }
    Effect()
        : name("Default"),
          triggerCondition([&]() -> bool { return true; }),
          segmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS),
          mode(FX_MODE_STATIC),
          reverse(false),
          transitionSpeed(INSTANT_TRANSITION),
          colors{0x000000, 0x000000, 0x000000},
          speed(255),
          fade(255),
          power(false),
          startTime(0),
          isRunning(false),
          runTime(0),
          remainingRuntime(0),
          palette(0),
          intensity(255),
          checksum("") // Ensure checksum is calculated last
    {
        interimTransitionSpeed = -1;
        checksum = calculateChecksum();
    }

    Effect(String n, std::function<bool()> tc, std::vector<int> seg, uint8_t m, bool rev, int ts, uint32_t c1, uint8_t s, uint8_t f, unsigned long r, uint8_t p, uint8_t i, bool pw = true)
        : name(n),
          triggerCondition(tc),
          segmentIDs(seg),
          mode(m),
          reverse(rev),
          transitionSpeed(ts),
          colors{c1, 0x000000, 0x000000},
          speed(s),
          fade(f),
          power(pw),
          startTime(0),
          isRunning(false),
          runTime(r),
          palette(p),
          intensity(i),
          checksum("") // Ensure checksum is calculated last
    {
        interimTransitionSpeed = -1;
        checksum = calculateChecksum();
    }
    /** Construct an effect from a Segment ID */
    Effect(int seg);

    // Builder methods
    Effect &setName(const String &n)
    {
        name = n;
        return *this;
    }

    Effect &setTriggerCondition(std::function<bool()> tc)
    {
        triggerCondition = tc;
        return *this;
    }

    Effect &setSegmentIDs(const std::vector<int> &seg)
    {
        segmentIDs = seg;
        return *this;
    }

    Effect &setMode(uint8_t m)
    {
        mode = m;
        return *this;
    }

    Effect &setTransitionSpeed(int ts)
    {
        transitionSpeed = ts;
        return *this;
    }
    Effect &setInterimTransitionSpeed(int its)
    {
        interimTransitionSpeed = its;
        return *this;
    }
    Effect &setColor(uint32_t c1, uint32_t c2 = 0x000000, uint32_t c3 = 0x000000)
    {
        colors[0] = c1;
        colors[1] = c2;
        colors[2] = c3;
        return *this;
    }

    Effect &setSpeed(uint8_t s)
    {
        speed = s;
        return *this;
    }

    Effect &setFade(uint8_t f)
    {
        fade = f;
        return *this;
    }

    Effect &setPower(bool p)
    {
        power = p;
        return *this;
    }

    Effect &setRunTime(unsigned long r)
    {
        runTime = r;
        return *this;
    }

    Effect &setPalette(uint8_t p)
    {
        palette = p;
        return *this;
    }

    Effect &setIntensity(uint8_t i)
    {
        intensity = i;
        return *this;
    }

    Effect &setReverse(bool r)
    {
        reverse = r;
        return *this;
    }

    Effect &setChecksum()
    {
        checksum = calculateChecksum();
        return *this;
    }
    void start(int segmentID = -1)
    {
        //Print the name of the effect and the segmentID
        p.print("Starting [" + name + "] on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);
        for (const auto &segmentID : segmentIDs)
        {
            p.print(String(segmentID) + " ", ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);
        }
        p.println(".", ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);

        if (segmentIDs.size() > 0)
        {
            ST->ensureRelayOn();
            triggerEffect(segmentID); // Actual function to start the effect
            isRunning = true;
        }
    }

    void stop()
    {
        // Code to stop the effect if necessary
        isRunning = false;
        if (segmentIDs.size() <= 0)
            return;

        p.print("Stopping [" + name + "] on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);
        for (const auto &segmentID : segmentIDs)
        {
            p.print(String(segmentID) + " ", ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);
        }
        p.println(".", ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);
    }
    bool checkTrigger()
    {
        // Call triggerCondition using ST from SubaruTelemetry and return boolean result
        return triggerCondition();

        // return (ST->*triggerCondition)();
    }
    void triggerEffect(int segmentID)
    {
        p.print("Triggering [" + name + "]  on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        // If segmentID is -1, trigger the effect on all segments
        // Else trigger the effect on only the specified segment
        std::vector<int> allSegments = segmentIDs;
        if (segmentID != -1)
        {
            allSegments = {segmentID};
        }
        for (const auto &segmentID : allSegments)
        {
            p.print(String(segmentID) + " ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
            uint32_t color1 = colors[0];
            uint32_t color2 = colors[1];
            uint32_t color3 = colors[2];
            bool on = power;
            if (!on)
            {
                color1 = 0x000000;
                color2 = 0x000000;
                color3 = 0x000000;
            }
            strip.setTransition(transitionSpeed);
            strip.setMode(segmentID, mode);
            strip.setBrightness(255, true);
            SegCon::seg(segmentID).setOption(SEG_OPTION_ON, on);
            SegCon::seg(segmentID).setOption(SEG_OPTION_REVERSED, reverse);
            SegCon::seg(segmentID).fade_out(fade);
            SegCon::seg(segmentID).intensity = intensity;
            SegCon::seg(segmentID).setColor(0, color1);
            SegCon::seg(segmentID).setColor(1, color2);
            SegCon::seg(segmentID).setColor(2, color3);
            SegCon::seg(segmentID).setPalette(palette);
            SegCon::seg(segmentID).speed = speed;
            SegCon::seg(segmentID).reverse = reverse;
            if(interimTransitionSpeed != -1){
                SegCon::seg(segmentID).startTransition(interimTransitionSpeed);
                interimTransitionSpeed = -1;
            }else{
                SegCon::seg(segmentID).startTransition(transitionSpeed);
            }
            strip.trigger();
        }
        p.println("for " + String(runTime) + " seconds...", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
    }
    bool isPreset(const std::array<String, 9> &presetChecksums) const
    {
        for (const auto &preset : presetChecksums)
        {
            if (checksum == preset)
            {
                return true;
            }
        }
        return false;
    }

private:
    String calculateChecksum() const
    {
        // Calculate the checksum, a string of the "mode", "colors", "speed", "palette", and "power" combined.
        //  This is used to identify the effect and prevent duplicates from being added to the queue
        // A single checksum should be generated from each segmentID

        String checksum = String(mode);
        checksum += ":" + String(name);
        checksum += ":" + String(colors[0]);
        checksum += ":" + String(speed);
        checksum += ":" + String(palette);
        checksum += ":" + String(power);
        checksum += ":" + String(intensity);

        return checksum;
    }
};
class EffectCollection
{
public:
    Effect doorOpen;
    Effect leftTurn;
    Effect rightTurn;
    Effect brake;
    Effect reverse;
    Effect ignition;
    Effect unlock;
    Effect lock;
    Effect off;

    std::array<String, 9> presetChecksums;

    EffectCollection(){
        doorOpen = Effect("Door Open")
            .setTriggerCondition([&]() -> bool { return ST->doorOpen.isInputActive(); })
            .setSegmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS)
            .setMode(FX_MODE_STATIC)
            .setTransitionSpeed(SLOW_TRANSITION)
            .setColor(0xFFC68C)
            .setSpeed(255)
            .setFade(255)
            .setRunTime(10000)
            .setPalette(0)
            .setIntensity(255)
            .setChecksum();

        leftTurn = Effect("Left Turn")
            .setTriggerCondition([&]() -> bool { return ST->left.isInputActive(); })
            .setSegmentIDs({LEFT_SEGMENT})
            .setMode(FX_MODE_LOADING)
            .setTransitionSpeed(INSTANT_TRANSITION)
            .setColor(0xFFAA00)
            .setSpeed(225)
            .setFade(255)
            .setRunTime(3000)
            .setPalette(0)
            .setIntensity(255)
            .setChecksum();

        rightTurn = Effect("Right Turn")
            .setTriggerCondition([&]() -> bool { return ST->right.isInputActive(); })
            .setSegmentIDs({RIGHT_SEGMENT})
            .setMode(FX_MODE_LOADING)
            .setTransitionSpeed(INSTANT_TRANSITION)
            .setColor(0xFFAA00)
            .setSpeed(225)
            .setFade(255)
            .setRunTime(3000)
            .setPalette(0)
            .setIntensity(255)
            .setChecksum();

        brake = Effect("Brake Engaged")
            .setTriggerCondition([&]() -> bool { return ST->brake.isInputActive(); })
            .setSegmentIDs({REAR_SEGMENT})
            .setMode(FX_MODE_STATIC)
            .setTransitionSpeed(INSTANT_TRANSITION)
            .setColor(0xFF0000)
            .setSpeed(255)
            .setFade(255)
            .setRunTime(0)
            .setPalette(0)
            .setIntensity(255)
            .setChecksum();

        reverse = Effect("Reverse Engaged")
            .setTriggerCondition([&]() -> bool { return ST->reverse.isInputActive(); })
            .setSegmentIDs({REAR_SEGMENT})
            .setMode(FX_MODE_STATIC)
            .setTransitionSpeed(INSTANT_TRANSITION)
            .setColor(0xFFC68C)
            .setSpeed(255)
            .setFade(255)
            .setRunTime(0)
            .setPalette(0)
            .setIntensity(255)
            .setChecksum();

        ignition = Effect("Ignition On")
            .setTriggerCondition([&]() -> bool { return ST->ignition.isInputActive(); })
            .setSegmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS)
            .setMode(FX_MODE_LOADING)
            .setTransitionSpeed(MEDIUM_TRANSITION)
            .setColor(0xFFC68C)
            .setSpeed(225)
            .setFade(255)
            .setRunTime(3000)
            .setPalette(0)
            .setIntensity(255)
            .setChecksum();

        lock = Effect("Car Locked")
            .setTriggerCondition([&]() -> bool { return ST->doorLock.isInputActive(); })
            .setSegmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS)
            .setMode(FX_MODE_GRADIENT)
            .setReverse(false)
            .setTransitionSpeed(SLOW_TRANSITION)
            .setColor(0xFF0000)
            .setSpeed(170)
            .setFade(255)
            .setRunTime(3000)
            .setPalette(0)
            .setIntensity(128)
            .setChecksum();

        unlock = Effect("Car Unlocked")
            .setTriggerCondition([&]() -> bool { return ST->doorUnlock.isInputActive(); })
            .setSegmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS)
            .setMode(FX_MODE_GRADIENT)
            .setReverse(true)
            .setTransitionSpeed(MEDIUM_TRANSITION)
            .setColor(0x39E75F)
            .setSpeed(170)
            .setFade(255)
            .setRunTime(3000)
            .setPalette(0)
            .setIntensity(128)
            .setChecksum();

        // unlock = Effect("Car Unlocked")
        //     .setTriggerCondition(&SubaruTelemetry::unlockedStatus)
        //     .setSegmentIDs(DEFAULT_SEGMENT_IDS)
        //     .setMode(FX_MODE_METEOR)
        //     .setReverse(true)
        //     .setTransitionSpeed(MEDIUM_TRANSITION)
        //     .setColor(0xFFAA00)
        //     .setSpeed(170)
        //     .setFade(255)
        //     .setRunTime(3000)
        //     .setPalette(63)
        //     .setIntensity(128)
        //     .setChecksum();

        off = Effect("Segment Off")
            .setTriggerCondition([&]() -> bool { return ST->ignition.isInputActive(); })
            .setSegmentIDs(DEFAULT_INTERIOR_SEGMENT_IDS)
            .setMode(FX_MODE_STATIC)
            .setTransitionSpeed(SLOW_TRANSITION)
            .setColor(0x000000)
            .setSpeed(255)
            .setFade(255)
            .setRunTime(0)
            .setPalette(0)
            .setIntensity(255)
            .setPower(false)
            .setChecksum();

        presetChecksums = {
            doorOpen.checksum,
            leftTurn.checksum,
            rightTurn.checksum,
            brake.checksum,
            reverse.checksum,
            ignition.checksum,
            unlock.checksum,
            lock.checksum,
            off.checksum
        };

        printAllChecksums();
    }
    /** Print all effect names and their corresponding checksums */
    void printAllChecksums() const
    {
        p.println("*****Checksums for all effects:******");
        p.println("doorOpen: " + String(doorOpen.checksum));
        p.println("leftTurn: " + String(leftTurn.checksum));
        p.println("rightTurn: " + String(rightTurn.checksum));
        p.println("brake: " + String(brake.checksum));
        p.println("reverse: " + String(reverse.checksum));
        p.println("ignition: " + String(ignition.checksum));
        p.println("unlock: " + String(unlock.checksum));
        p.println("lock: " + String(lock.checksum));
        p.println("off: " + String(off.checksum));
        p.println("*************************************");
    }
    bool isPreset(const Effect *effect) const
    {
        for (const auto &presetChecksum : presetChecksums)
        {
            if (effect->checksum == presetChecksum)
            {
                return true;
            }
        }
        return false;
    }
    bool checkSegmentIntegrity(){
        return SC.checkSegmentIntegrity();
    }
};

EffectCollection effects;


Effect::Effect(int seg)
{
    name = String("EFS" + String(seg));
    Segment &segment = SegCon::seg(seg);
    segmentIDs.push_back(seg);
    mode = segment.mode;
    reverse = segment.reverse;
    transitionSpeed = strip.getTransition();
    speed = segment.speed;
    fade = 255;
    power = segment.on;
    startTime = 0;
    isRunning = true;
    runTime = 0;
    palette = segment.palette;
    intensity = segment.intensity;
    colors[0] = segment.colors[0];
    colors[1] = segment.colors[1];
    colors[2] = segment.colors[2];
    triggerCondition = [&]() -> bool { return true; };
    checksum = calculateChecksum();
    // If checksum matches a preset, return a reference to the preset
    if (isPreset(effects.presetChecksums))
    {
        if (checksum == effects.doorOpen.checksum)
        {
            *this = effects.doorOpen;
        }
        else if (checksum == effects.leftTurn.checksum)
        {
            *this = effects.leftTurn;
        }
        else if (checksum == effects.rightTurn.checksum)
        {
            *this = effects.rightTurn;
        }
        else if (checksum == effects.brake.checksum)
        {
            *this = effects.brake;
        }
        else if (checksum == effects.reverse.checksum)
        {
            *this = effects.reverse;
        }
        else if (checksum == effects.ignition.checksum)
        {
            *this = effects.ignition;
        }
        else if (checksum == effects.unlock.checksum)
        {
            *this = effects.unlock;
        }
        else if (checksum == effects.lock.checksum)
        {
            *this = effects.lock;
        }
        else if (checksum == effects.off.checksum)
        {
            *this = effects.off;
        }
    }
};
#endif