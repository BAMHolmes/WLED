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
enum Priority
{
    SubaruFirst,
    SubaruSecond,
    SubaruThird,
    SubaruFourth,
    SubaruFifth,
    SubaruSixth
};
class EffectSegmentStatus
{
public:
    int segmentID;
    unsigned long startTime;
    bool expired;
    bool power;
    EffectSegmentStatus()
    {
        segmentID = -1;
        startTime = 0;
        expired = false;
        power = false;
    }
    EffectSegmentStatus(int segID, unsigned long st, bool ex, bool pw)
    {
        segmentID = segID;
        startTime = st;
        expired = ex;
        power = pw;
    }
};
/**
 * @brief Class for managing the effect for each segment.
 */

class Effect
{
public:
    // using TriggerCondition = bool (PinState::*)() const;

    String name;
    std::function<bool()> triggerCondition;
    std::vector<int> segmentIDs;
    int boundSegmentID;
    std::vector<Effect *> queuedEffects;
    std::map<int, EffectSegmentStatus> segmentStatusMap;
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
    bool foreground;
    unsigned long runTime;
    unsigned long remainingRuntime;
    uint8_t palette;
    uint8_t intensity;
    uint8_t priority;
    String checksum;
    String weakChecksum;
    bool isPreset(bool);
    bool isStarted(int segmentID = -1)
    {
        if (segmentID != -1)
        {
            return segmentStatusMap[segmentID].startTime > 0;
        }
        return startTime > 0;
    }
    bool iHaveSegments()
    {
        return segmentIDs.size() > 0;
    }
    bool sameAs(Effect *effect)
    {
        if (effect == nullptr || checksum.isEmpty() || effect->checksum.isEmpty()) return false;
        return checksum == effect->checksum || weakChecksum == effect->weakChecksum;
    }
    Effect(String n)
        : name(n),
          triggerCondition([this]() -> bool
                           { return true; }),
          segmentIDs(ALL_SUBARU_SEGMENT_IDS),
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
          priority(0),
          checksum(""), // Ensure checksum is calculated last
          weakChecksum("")
    {
        reverse = false;
        boundSegmentID = -1;
        interimTransitionSpeed = -1;
        checksum = calculateChecksum();
        weakChecksum = calculateChecksum(false);
    }
    Effect()
        : name("Default"),
          triggerCondition([this]() -> bool
                           { return true; }),
          segmentIDs(ALL_SUBARU_SEGMENT_IDS),
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
          priority(0),
          checksum(""), // Ensure checksum is calculated last
          weakChecksum("")
    {
        boundSegmentID = -1;
        interimTransitionSpeed = -1;
        checksum = calculateChecksum();
        weakChecksum = calculateChecksum(false);
    }

    Effect(String n, std::function<bool()> tc, std::vector<int> seg, uint8_t m, bool rev, int ts, uint32_t c1, uint8_t s, uint8_t f, unsigned long r, uint8_t p, uint8_t i, uint8_t pri, bool pw = true)
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
          priority(pri),
          checksum(""), // Ensure checksum is calculated last
          weakChecksum("")
    {
        boundSegmentID = -1;
        interimTransitionSpeed = -1;
        checksum = calculateChecksum();
        weakChecksum = calculateChecksum(false);
    }
    Effect(const Effect& other)
            : name(other.name),
            triggerCondition(other.triggerCondition), // Assuming std::function can be copied safely
            segmentIDs(other.segmentIDs),
            boundSegmentID(other.boundSegmentID),
            segmentStatusMap(other.segmentStatusMap), // Deep copy map of custom objects
            mode(other.mode),
            reverse(other.reverse),
            transitionSpeed(other.transitionSpeed),
            speed(other.speed),
            fade(other.fade),
            power(other.power),
            startTime(other.startTime),
            isRunning(other.isRunning),
            runTime(other.runTime),
            palette(other.palette),
            intensity(other.intensity),
            priority(other.priority),
            checksum(other.checksum),
            weakChecksum(other.weakChecksum)
        {
            std::copy(std::begin(other.colors), std::end(other.colors), std::begin(colors));  // Copy array contents
        }
    Effect& operator=(const Effect& other) {
        if (this != &other) {
            // Manually copy each field from `other` to `this`
            name = other.name;
            triggerCondition = other.triggerCondition;  // Assuming std::function can be copied safely
            segmentIDs = other.segmentIDs;
            boundSegmentID = other.boundSegmentID;
            queuedEffects = other.queuedEffects;  // Deep copy might be necessary depending on usage
            segmentStatusMap = other.segmentStatusMap;  // Deep copy map of custom objects
            mode = other.mode;
            reverse = other.reverse;
            transitionSpeed = other.transitionSpeed;
            interimTransitionSpeed = other.interimTransitionSpeed;
            std::copy(std::begin(other.colors), std::end(other.colors), std::begin(colors));  // Copy array contents
            speed = other.speed;
            fade = other.fade;
            power = other.power;
            startTime = other.startTime;
            isRunning = other.isRunning;
            foreground = other.foreground;
            runTime = other.runTime;
            remainingRuntime = other.remainingRuntime;
            palette = other.palette;
            intensity = other.intensity;
            priority = other.priority;
            checksum = other.checksum;
            weakChecksum = other.weakChecksum;
        }
        return *this;
    }

    void morph();
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
        // Loop through all segmentIDs and create a map of segmentID to EffectSegmentStatus
        for (int segmentID : segmentIDs)
        {
            segmentStatusMap[segmentID] = EffectSegmentStatus(segmentID, 0, false, false);
        }
        return *this;
    }
    Effect &setPriority(uint8_t pri)
    {
        priority = pri;
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
        // Loop through and set power of each segmentStatusMap
        for (auto &segmentStatus : segmentStatusMap)
        {
            segmentStatus.second.power = p;
        }
        return *this;
    }
    Effect &resetStartTime(int segmentID = -1)
    {
        startTime = 0;
        // If segmentID is -1, reset the start time for all segmentStatusMap
        if (segmentID == -1)
        {
            for (auto &segmentStatus : segmentStatusMap)
            {
                segmentStatus.second.startTime = 0;
            }
        }
        else
        {
            // If segmentID is not -1, reset the start time for the specified segmentID
            segmentStatusMap[segmentID].startTime = 0;
        }
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
        weakChecksum = calculateChecksum(false);
        return *this;
    }
    bool mySegment(int segmentID)
    {
        bool isMySegment = false;
        for (int id : segmentIDs)
        {
            if (id == segmentID)
            {
                isMySegment = true;
                break;
            }
        }
        return isMySegment;
    }
    void run(int segmentID = -1)
    {
        bool isTriggering = triggering();
        bool isRunning = (startTime > 0) && (millis() - startTime) < runTime;
        ensureRunning();
    }
    void ensureRunning()
    {
        // for (const auto &segmentID : segmentIDs)
        // {
        //     SubaruSegment seg = SegCon::seg(segmentID);
        //     // Check the segment to see what's currently running...
        //     Effect *currentEffect = ;
        //     bool itsMe = currentEffect->weakChecksum == weakChecksum;
        //     bool relayOn = seg.relay->isOutputActive();

        //     if (!itsMe)
        //     {
        //         // If it's not me, push the effect to my stack
        //         queuedEffects.push_back(currentEffect);
        //         triggerEffect(segmentID); // Actual function to start the effect
        //     }
        //     if (!relayOn)
        //     {
        //         seg.activateRelay();
        //         //p->print(String(name) + " relay was off. Turning on the relay for segment-" + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);
        //     }
        // }
    }
    void start(int segmentID = -1)
    {
        startTime = millis();
        if (segmentID != -1)
        {
            segmentStatusMap[segmentID].startTime = startTime;
            segmentStatusMap[segmentID].expired = false;
        }
        SubaruSegment seg = SegCon::seg(segmentID);
        if(!seg.relay->isOutputActive()){
            seg.activateRelay();
        }

    }
    void stop(int segmentID, Effect *nextEffect)
    {
        // Code to stop the effect if necessary
        power = false;
        startTime = 0;
        if (segmentID != -1)
        {
            segmentStatusMap[segmentID].startTime = 0;
            segmentStatusMap[segmentID].expired = false;
        }
        ////p->print("Stopping [" + name + "] on segment " + segmentID + ". Up next: " + nextEffect->name, ColorPrint::FG_WHITE, ColorPrint::BG_GRAY);

        nextEffect->triggerEffect(segmentID);
        // SubaruSegment seg = SegCon::getInstance()->getSegment(segmentID);
    }
    bool triggering()
    {
        // Call triggerCondition using ST from SubaruTelemetry and return boolean result
        return triggerCondition();

        // return (ST->*triggerCondition)();
    }
    void triggerEffect(int segmentID, bool force = false)
    {
        if (!isStarted(segmentID) || force)
        {
            start(segmentID);
        }
        //p->println("Triggering [" + name + "]  on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        // If segmentID is -1, trigger the effect on all segments
        // Else trigger the effect on only the specified segment
        std::vector<int> allSegments = segmentIDs;
        if (segmentID != -1)
        {
            allSegments = {segmentID};
        }
        for (const auto &segmentID : allSegments)
        {
            ////p->print(String(segmentID) + " ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
            uint32_t color1 = colors[0];
            uint32_t color2 = colors[1];
            uint32_t color3 = colors[2];
            bool on = power || segmentStatusMap[segmentID].power;
            if (!on)
            {
                //Attempt to print a backtrace at this point

                
                //p->println("Turning off segment " + String(segmentID) + "...", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
                
                //delay(10000);

                color1 = 0x000000;
                color2 = 0x000000;
                color3 = 0x000000;
            }
            strip.setTransition(transitionSpeed);
            strip.setMode(segmentID, mode);
            strip.setBrightness(255, true);
            strip.getSegment(segmentID).setOption(SEG_OPTION_ON, on);
            strip.getSegment(segmentID).setOption(SEG_OPTION_REVERSED, reverse);
            strip.getSegment(segmentID).fade_out(fade);
            strip.getSegment(segmentID).intensity = intensity;
            strip.getSegment(segmentID).setColor(0, color1);
            strip.getSegment(segmentID).setColor(1, color2);
            strip.getSegment(segmentID).setColor(2, color3);
            strip.getSegment(segmentID).setPalette(palette);
            strip.getSegment(segmentID).speed = speed;
            strip.getSegment(segmentID).reverse = reverse;
            if (interimTransitionSpeed != -1)
            {
                strip.getSegment(segmentID).startTransition(interimTransitionSpeed);
                interimTransitionSpeed = -1;
            }
            else
            {
                strip.getSegment(segmentID).startTransition(transitionSpeed);
            }
            strip.trigger();
        }
        //p->println("for " + String(runTime) + " seconds...", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
    }
    bool isExpired(int segmentID = -1)
    {
        if(boundSegmentID != -1){
            return false;
        }
        unsigned long _startTime = startTime;
        if (segmentID != -1)
        {
            _startTime = segmentStatusMap[segmentID].startTime;
        }
        bool _isExpired = isStarted(segmentID) && (millis() - _startTime) > runTime;
        if (segmentID != -1)
        {
            segmentStatusMap[segmentID].expired = _isExpired;
        }
        return _isExpired;
    }

    // bool isPreset(const std::array<String, 9> &presetChecksums) const
    // {
    //     for (const auto &preset : presetChecksums)
    //     {
    //         if (checksum == preset)
    //         {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

private:
    String calculateChecksum(bool useName = true) const
    {
        // Calculate the checksum, a string of the "mode", "colors", "speed", "palette", and "power" combined.
        //  This is used to identify the effect and prevent duplicates from being added to the queue
        // A single checksum should be generated from each segmentID

        String checksum = String(mode);
        //Create a string of all segmentIDs by looping through array
        checksum += ":seg[";
        for (const auto &segmentID : segmentIDs)
        {
            checksum += String(segmentID);
        }
        
        if (useName)
        {
            checksum += ":" + String(name);
        }
        checksum += ":[c0=" + String(colors[0]) + ";c1=" + String(colors[1]) + ";c2=" + String(colors[2]) + "]";
        checksum += ":speed=" + String(speed);
        checksum += ":palette=" + String(palette);
        checksum += ":power=" + String(power);
        checksum += ":intesity=" + String(intensity);

        return checksum;
    }
};
std::map<int, Effect *> EFFECTS_NOW;

class EffectCollection
{
public:
    Effect park;
    Effect doorOpen;
    Effect leftTurn;
    Effect rightTurn;
    Effect brake;
    Effect reverse;
    Effect ignition;
    Effect unlock;
    Effect lock;
    Effect off;
    Effect generic;
    std::vector<Effect *> allEffects;
    std::array<String, 9> presetChecksums;
    std::map<String, Effect *> effectMap;
    std::map<String, Effect *> weakEffectMap;

    EffectCollection()
    {
        initEffects();
        effectMap[park.checksum] = &park;
        effectMap[leftTurn.checksum] = &leftTurn;
        effectMap[rightTurn.checksum] = &rightTurn;
        effectMap[brake.checksum] = &brake;
        effectMap[reverse.checksum] = &reverse;
        effectMap[ignition.checksum] = &ignition;
        effectMap[unlock.checksum] = &unlock;
        effectMap[lock.checksum] = &lock;
        effectMap[off.checksum] = &off;

        weakEffectMap[park.weakChecksum] = &park;
        weakEffectMap[doorOpen.weakChecksum] = &doorOpen;
        weakEffectMap[leftTurn.weakChecksum] = &leftTurn;
        weakEffectMap[rightTurn.weakChecksum] = &rightTurn;
        weakEffectMap[brake.weakChecksum] = &brake;
        weakEffectMap[reverse.weakChecksum] = &reverse;
        weakEffectMap[ignition.weakChecksum] = &ignition;
        weakEffectMap[unlock.weakChecksum] = &unlock;
        weakEffectMap[lock.weakChecksum] = &lock;
        weakEffectMap[off.weakChecksum] = &off;
    }
    void initEffects()
    {
        park = Effect("Car Parked")
                    .setTriggerCondition([this]() -> bool
                                        { return ST->parked.isInputActive(); })
                    .setSegmentIDs(SUBARU_GROUND_SEGMENT_IDS)
                    .setMode(FX_MODE_STATIC)
                    .setTransitionSpeed(SLOW_TRANSITION)
                    .setColor(0xFFC68C)
                    .setSpeed(255)
                    .setFade(255)
                    .setRunTime(0)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruFirst)
                    .setChecksum();
        doorOpen = Effect("Door Open")
                    .setTriggerCondition([this]() -> bool
                                        { return ST->doorOpen.isInputActive(); })
                    .setSegmentIDs(ALL_SUBARU_SEGMENT_IDS)
                    .setMode(FX_MODE_STATIC)
                    .setTransitionSpeed(SLOW_TRANSITION)
                    .setColor(0xFFC68C)
                    .setSpeed(255)
                    .setFade(255)
                    .setRunTime(30000)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruSecond)
                    .setChecksum();

        leftTurn = Effect("Left Turn")
                    .setTriggerCondition([this]() -> bool
                                        { return ST->left.isInputActive(); })
                    .setSegmentIDs({LEFT_SEGMENT})
                    .setMode(FX_MODE_LOADING)
                    .setTransitionSpeed(INSTANT_TRANSITION)
                    .setColor(0xFFAA00)
                    .setSpeed(225)
                    .setFade(255)
                    .setRunTime(3000)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruThird)
                    .setChecksum();

        rightTurn = Effect("Right Turn")
                    .setTriggerCondition([this]() -> bool
                                            { return ST->right.isInputActive(); })
                    .setSegmentIDs({RIGHT_SEGMENT})
                    .setMode(FX_MODE_LOADING)
                    .setTransitionSpeed(INSTANT_TRANSITION)
                    .setColor(0xFFAA00)
                    .setSpeed(225)
                    .setFade(255)
                    .setRunTime(3000)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruThird)
                    .setChecksum();

        brake = Effect("Brake Engaged")
                    .setTriggerCondition([this]() -> bool
                                         { return ST->brake.isInputActive(); })
                    .setSegmentIDs({REAR_SEGMENT})
                    .setMode(FX_MODE_STATIC)
                    .setTransitionSpeed(INSTANT_TRANSITION)
                    .setColor(0xFF0000)
                    .setSpeed(255)
                    .setFade(255)
                    .setRunTime(0)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruFirst)
                    .setChecksum();

        reverse = Effect("Reverse Engaged")
                    .setTriggerCondition([this]() -> bool
                                        { return ST->reverse.isInputActive(); })
                    .setSegmentIDs({REAR_SEGMENT})
                    .setMode(FX_MODE_STATIC)
                    .setTransitionSpeed(INSTANT_TRANSITION)
                    .setColor(0xFFC68C)
                    .setSpeed(255)
                    .setFade(255)
                    .setRunTime(0)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruSixth)
                    .setChecksum();

        ignition = Effect("Ignition On")
                    .setTriggerCondition([this]() -> bool
                                        { return ST->ignition.activated(); })
                    .setSegmentIDs(ALL_SUBARU_SEGMENT_IDS)
                    .setMode(FX_MODE_LOADING)
                    .setTransitionSpeed(MEDIUM_TRANSITION)
                    .setColor(0xFFC68C)
                    .setSpeed(225)
                    .setFade(255)
                    .setRunTime(3000)
                    .setPower(true)
                    .setPalette(0)
                    .setIntensity(255)
                    .setPriority(Priority::SubaruFifth)
                    .setChecksum();

        lock = Effect("Car Locked")
                   .setTriggerCondition([this]() -> bool
                                        { return ST->doorLock.isInputActive(); })
                   .setSegmentIDs(ALL_SUBARU_SEGMENT_IDS)
                   .setMode(FX_MODE_GRADIENT)
                   .setReverse(false)
                   .setTransitionSpeed(SLOW_TRANSITION)
                   .setColor(0xFF0000)
                   .setSpeed(170)
                   .setFade(255)
                   .setRunTime(3000)
                   .setPalette(0)
                   .setPower(true)
                   .setIntensity(128)
                   .setPriority(Priority::SubaruFourth)
                   .setChecksum();

        unlock = Effect("Car Unlocked")
                    .setTriggerCondition([this]() -> bool
                                        { return ST->doorUnlock.isInputActive(); })
                    .setSegmentIDs(ALL_SUBARU_SEGMENT_IDS)
                    .setMode(FX_MODE_GRADIENT)
                    .setReverse(true)
                    .setTransitionSpeed(MEDIUM_TRANSITION)
                    .setColor(0x39E75F)
                    .setSpeed(170)
                    .setFade(255)
                    .setRunTime(3000)
                    .setPalette(0)
                    .setPower(true)
                    .setIntensity(128)
                    .setPriority(Priority::SubaruFourth)
                    .setChecksum();

        generic = Effect("Morph")
                    .setTriggerCondition([this]() -> bool
                                        { return true; })
                    .setSegmentIDs(ALL_SUBARU_SEGMENT_IDS)
                    .setMode(FX_MODE_STATIC)
                    .setTransitionSpeed(SLOW_TRANSITION)
                    .setColor(0x000000)
                    .setSpeed(255)
                    .setFade(255)
                    .setRunTime(0)
                    .setPalette(0)
                    .setIntensity(255)
                    .setPower(false)
                    .setPriority(Priority::SubaruSixth)
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
                .setTriggerCondition([this]() -> bool
                                    { return ST->ignition.isInputInactive(); })
                .setSegmentIDs(ALL_SUBARU_SEGMENT_IDS)
                .setMode(FX_MODE_STATIC)
                .setTransitionSpeed(SLOW_TRANSITION)
                .setColor(0x000000)
                .setSpeed(255)
                .setFade(255)
                .setRunTime(0)
                .setPalette(0)
                .setIntensity(255)
                .setPower(false)
                .setPriority(Priority::SubaruSixth)
                .setChecksum();

        presetChecksums = {
            park.checksum,
            doorOpen.checksum,
            leftTurn.checksum,
            rightTurn.checksum,
            brake.checksum,
            reverse.checksum,
            ignition.checksum,
            unlock.checksum,
            lock.checksum,
        };

        allEffects = {&park, &doorOpen, &leftTurn, &rightTurn, &brake, &reverse, &ignition, &unlock, &lock};
        printAllChecksums();
    }
    /** Generate a new effect based on effects.generic */
    Effect *generateEffect(int segmentID = -1)
    {
        Effect *newEffect = new Effect();
        if (segmentID != -1)
        {
            newEffect->boundSegmentID = segmentID;
        }
        return newEffect;
    }
    /** Print all effect names and their corresponding checksums */

    void printAllChecksums() const
    {
        //p->println("*****Checksums for all effects:******");
        //p->println("doorOpen: " + String(doorOpen.checksum));
        //p->println("leftTurn: " + String(leftTurn.checksum));
        //p->println("rightTurn: " + String(rightTurn.checksum));
        //p->println("brake: " + String(brake.checksum));
        //p->println("reverse: " + String(reverse.checksum));
        //p->println("ignition: " + String(ignition.checksum));
        //p->println("unlock: " + String(unlock.checksum));
        //p->println("lock: " + String(lock.checksum));
        //p->println("off: " + String(off.checksum));
        //p->println("*************************************");
    }
    bool isPreset(const Effect *effect, bool debug = false) const
    {
        String theWeakChecksum = effect->weakChecksum;
        //Check weakChecksumMap for match by explicitly looping through the map and comparing the checksum
        for(auto pair : weakEffectMap){
            if(debug){
            //p->print("Comparing [" + pair.first + "] to [" + theWeakChecksum + "]", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);

            }
            if(pair.first == theWeakChecksum){
                if(debug){
                //p->println(" TRUE ", ColorPrint::FG_BLACK, ColorPrint::BG_GREEN);
                }
                return true;
            }
            if(debug){
            //p->println(" FALSE ", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
            }
        }
        if(debug){
        //p->println("No match found for [" + theWeakChecksum + "]", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
        }
        return false;
    }
    bool checkSegmentIntegrity()
    {
        return SegCon::getInstance()->checkSegmentIntegrity();
    }
    // Get an effect by its checksum
    // Get an effect by its checksum
    Effect *getEffectByChecksum(const String &checksum)
    {
        // Choose the correct map based on the weak flag
        auto weakMatch = weakEffectMap.find(checksum);
        auto match = effectMap.find(checksum);
        if (match != effectMap.end())
        {
            return match->second;
        }
        if (weakMatch != weakEffectMap.end())
        {
            return weakMatch->second;
        }
        return nullptr;
    }
};

EffectCollection effects;

bool Effect::isPreset(bool debug = false)
{
    return effects.isPreset(this, debug);
}
/** Construct an effect from a Segment ID */
void Effect::morph()
{

    if (strip._segments.size() < 8 || boundSegmentID < 0 || boundSegmentID >= strip._segments.size()) {
        return;
    }

    Segment* segment = &strip.getSegment(boundSegmentID);
    bool isUpdating = strip.isUpdating();
    if(isUpdating){
        //p->println("Segment is updating. Skipping morph...", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
        return;
    }
    ////p->println("\t\tMorphing effect for segment " + String(boundSegmentID) + " using effect [" + name + "]", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
    setMode(segment->mode);
    setReverse(segment->reverse);
    setColor(segment->colors[0], segment->colors[1], segment->colors[2]);
    setSpeed(segment->speed);
    setPower(segment->on);
    setPalette(segment->palette);
    setIntensity(segment->intensity);
    setRunTime(0); // Default to continuous run unless specified
    ////p->println("\t\tSegment mode is " + String(segment->mode) + " (" + String(strip.getModeData(segment->mode)) + ")", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
    std::string mode_data = strip.getModeData(segment->mode);
    String _name = std::string(mode_data).substr(0, mode_data.find('@')).c_str();
    ////p->println("\t\tSetting name to [" + _name + "]", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);

    setName(_name);
    setChecksum(); // Recalculate checksum based on the current state

    // Check if checksum matches a preset, if so return a reference to the preset
    // Effect *preset = effects.getEffectByChecksum(checksum);
    // if (preset != nullptr)
    // {
    //     //p->println("!!!!!!!!!!!Effect [" + name + "] is a preset effect: " + preset->name, ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
    //     return *preset;
    // }
}

#endif