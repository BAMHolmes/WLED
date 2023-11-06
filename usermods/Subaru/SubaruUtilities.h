#include "wled.h"
#include "SubaruTelemetry.h"
#include <cstdint>
#include <array>
#include <unordered_map>
#include <map>

class Overrides
{
public:
    // Constructors
    Overrides() : Brake(false), Door(false), DoorsUnlocked(false), DoorsLocked(false), Reverse(false), LeftIndicator(false), RightIndicator(false), Ignition(false) {}
    Overrides(bool brake, bool door, bool doorsUnlocked, bool doorsLocked, bool reverse, bool leftIndicator, bool rightIndicator, bool ignition)
        : Brake(brake), Door(door), DoorsUnlocked(doorsUnlocked), DoorsLocked(doorsLocked), Reverse(reverse), LeftIndicator(leftIndicator), RightIndicator(rightIndicator), Ignition(ignition) {}

    // Getters
    bool getBrake() const { return Brake; }
    bool getDoor() const { return Door; }
    bool getLeftIndicator() const { return LeftIndicator; }
    bool getRightIndicator() const { return RightIndicator; }
    bool getReverse() const { return Reverse; }
    bool getDoorsUnlocked() const { return DoorsUnlocked; }
    bool getDoorsLocked() const { return DoorsLocked; }
    bool getIgnition() const { return Ignition; }

    // Setters
    void setBrake(bool brake) { Brake = brake; }
    void setDoor(bool door) { Door = door; }
    void setDoorsUnlocked(bool doorsUnlocked) { DoorsUnlocked = doorsUnlocked; }
    void setDoorsLocked(bool doorsLocked) { DoorsLocked = doorsLocked; }
    void setReverse(bool reverse) { Reverse = reverse; }
    void setLeftIndicator(bool leftIndicator) { LeftIndicator = leftIndicator; }
    void setRightIndicator(bool rightIndicator) { RightIndicator = rightIndicator; }
    void setIgnition(bool ignition) { Ignition = ignition; }
    bool globalIsClear()
    {
        return !Door && !DoorsUnlocked && !DoorsLocked && !Ignition;
    }

    bool checkOtherGlobals(String global)
    {
        if (global = "door")
        {
            return DoorsUnlocked || DoorsLocked || Ignition;
        }
        if (global = "doorsUnlocked")
        {
            return Door || DoorsLocked || Ignition;
        }
        if (global = "doorsLocked")
        {
            return Door || DoorsUnlocked || Ignition;
        }
        if (global = "ignition")
        {
            return Door || DoorsUnlocked || DoorsLocked;
        }
    }
    bool checkOthers(String effect)
    {
        if (effect == "brake")
        {
            return Door || DoorsUnlocked || DoorsLocked || Reverse || LeftIndicator || RightIndicator;
        }
        if (effect == "door")
        {
            return Brake || DoorsUnlocked || DoorsLocked || Reverse || LeftIndicator || RightIndicator;
        }
        if (effect == "doorsUnlocked")
        {
            return Brake || Door || DoorsLocked || Reverse || LeftIndicator || RightIndicator;
        }
        if (effect == "doorsLocked")
        {
            return Brake || Door || DoorsUnlocked || Reverse || LeftIndicator || RightIndicator;
        }
        if (effect == "reverse")
        {
            return Brake || Door || DoorsUnlocked || DoorsLocked || LeftIndicator || RightIndicator;
        }
        if (effect == "leftIndicator")
        {
            return Brake || Door || DoorsUnlocked || DoorsLocked || Reverse || RightIndicator;
        }
        if (effect == "rightIndicator")
        {
            return Brake || Door || DoorsUnlocked || DoorsLocked || Reverse || LeftIndicator;
        }
        return false;
    }
    bool Brake;
    bool Door;
    bool DoorsUnlocked;
    bool DoorsLocked;
    bool Reverse;
    bool LeftIndicator;
    bool RightIndicator;
    bool Ignition;
};
class Effect
{
public:
    uint8_t mode;
    uint32_t colors[3];
    uint8_t speed;
    uint8_t fade;
    uint8_t palette;
    bool power;
    uint32_t checksum;

    Effect() : mode(FX_MODE_STATIC), speed(255), fade(255), palette(0), power(false), checksum(0)
    {
        // You can set default values here
        std::fill(std::begin(colors), std::end(colors), 0x000000);
        checksum = calculateChecksum();
    }

    Effect(uint8_t m, uint32_t c1, uint8_t s, uint8_t f, uint8_t p, bool pw = true)
        : mode(m), colors{c1, 0x000000, 0x000000}, speed(s), fade(f), palette(p), power(pw)
    {
        checksum = calculateChecksum();
    }

    Effect(uint8_t m, uint32_t c1, uint32_t c2, uint32_t c3, uint8_t s, uint8_t f, uint8_t p, bool pw = true)
        : mode(m), colors{c1, c2, c3}, speed(s), fade(f), palette(p), power(pw)
    {
        checksum = calculateChecksum();
    }
    Effect(const Segment& segment)
        : mode(segment.mode),
          colors{segment.colors[0], segment.colors[1], segment.colors[2]}, // Assuming default colors for color2 and color3
          speed(segment.speed),
          fade(255),
          palette(segment.palette),
          power(segment.getOption(SEG_OPTION_ON))
    {
        checksum = calculateChecksum();
    }
    bool isPreset(const std::array<uint32_t, 9> &presetChecksums) const
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
    uint32_t calculateChecksum() const
    {
        uint32_t checksum = mode;
        checksum += colors[0] + colors[1] + colors[2];
        checksum += speed;
        checksum += fade;
        checksum += palette;
        checksum += static_cast<uint32_t>(power);
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
    std::array<uint32_t, 9> presetChecksums;

    EffectCollection() : doorOpen(Effect(FX_MODE_STATIC, 0xFFC68C, 255, 255, 0)),
                         leftTurn(Effect(FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 0, 0)),
                         rightTurn(Effect(FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 255, 0)),
                         brake(Effect(FX_MODE_STATIC, 0xFF0000, 255, 255, 0)),
                         reverse(Effect(FX_MODE_STATIC, 0xFFC68C, 255, 255, 0)),
                         ignition(Effect(FX_MODE_LOADING, 0xFFC68C, 0x000000, 0x000000, 225, 255, 0)),
                         unlock(Effect(FX_MODE_BLINK, 0xFFAA00, 200, 255, 0)),
                         lock(Effect(FX_MODE_BLINK, 0xFF0000, 200, 255, 0)),
                         off(Effect(FX_MODE_STATIC, 0x000000, 255, 255, 0))
    {
        presetChecksums = {
            doorOpen.checksum,
            leftTurn.checksum,
            rightTurn.checksum,
            brake.checksum,
            reverse.checksum,
            ignition.checksum,
            unlock.checksum,
            lock.checksum,
            off.checksum};
    }

    bool isPreset(const Effect& effect) const
    {
        for (const auto& presetChecksum : presetChecksums)
        {
            if (effect.checksum == presetChecksum)
            {
                return true;
            }
        }
        return false;
    }    
};

class EffectCache
{
private:
    std::map<int, std::unordered_map<uint32_t, Effect>> segmentEffectMap;

public:
    Effect LeftSegment;
    Effect RightSegment;
    Effect RearSegment;
    Effect FrontSegment;
    Effect Default = Effect(FX_MODE_STATIC, 0x000000, 255, 255, 0);
    SubaruTelemetry ST = SubaruTelemetry();

    EffectCache() : LeftSegment(Default),
                    RightSegment(Default),
                    RearSegment(Default),
                    FrontSegment(Default)
    {
        refresh();
    }
    Effect getBySegment(int segId){
        switch (segId)
        {
        case LEFT_SEGMENT:
            return LeftSegment;
            break;
        case RIGHT_SEGMENT:
            return RightSegment;
            break;
        case BRAKE_SEGMENT:
            return RearSegment;
            break;
        case FRONT_SEGMENT:
            return FrontSegment;
            break;
        default:
            return Default;
            break;
        }
    }
    Effect setBySegment(int segId, Effect effect){
        switch (segId)
        {
        case LEFT_SEGMENT:
            LeftSegment = effect;
            break;
        case RIGHT_SEGMENT:
            RightSegment = effect;
            break;
        case BRAKE_SEGMENT:
            RearSegment = effect;
            break;
        case FRONT_SEGMENT:
            FrontSegment = effect;
            break;
        default:
            return Default;
            break;
        }
    }

    void setBySegmentAndChecksum(int segId, uint32_t checksum, const Effect& effect) {
        segmentEffectMap[segId][checksum] = effect;
    }
    //Overload setBySegmentAndChecksum to accept segId and effect, then extract checksum from effect
    void setBySegmentAndChecksum(int segId, const Effect& incomingEffect, const Effect& outgoingEffect) {
        setBySegmentAndChecksum(segId, incomingEffect.checksum, outgoingEffect);
    }

    const Effect* getBySegmentAndChecksum(int segId, uint32_t checksum) const {
        const auto segmentIt = segmentEffectMap.find(segId);
        if (segmentIt != segmentEffectMap.end()) {
            const auto& effectMap = segmentIt->second;
            const auto effectIt = effectMap.find(checksum);
            if (effectIt != effectMap.end()) {
                return &effectIt->second;
            }
        }
        return nullptr; // Return nullptr if no Effect is found
    }
    //Overload getBySegmentAndChecksum to accept segId and effect, then extract checksum from effect
    const Effect* getBySegmentAndChecksum(int segId, const Effect& effect) const {
        return getBySegmentAndChecksum(segId, effect.checksum);
    }


    bool isSegmentInitialized(int seg = LEFT_SEGMENT)
    {
        const auto &segments = strip.getSegments();
        if (segments == nullptr)
        {
            Serial.println("Error: Program running too early. Segments array is not initialized");
            return false;
        }
        const Segment &leftSeg = segments[seg];

        // Additional checks can be added here depending on the properties of Segment
        // For example, checking if colors array is valid
        if (leftSeg.colors == nullptr)
        {
            Serial.println("Error: Program running too early. Segment colors are not initialized");
            return false;
        }
        return true;
    }
    bool setLeft()
    {
        if (!isSegmentInitialized(LEFT_SEGMENT))
        {
            return false;
        }
        auto &left = ST.leftSegment();
        LeftSegment = Effect(left.mode, left.colors[0], left.colors[1], left.colors[2], left.speed, left.palette, left.getOption(SEG_OPTION_ON));
        return true;
    }

    bool setLeft(Effect leftSeg)
    {
        LeftSegment = leftSeg;
        return true;
    }
    bool setRight()
    {
        if (!isSegmentInitialized(RIGHT_SEGMENT))
        {
            return false;
        }
        auto &right = ST.rightSegment();
        RightSegment = Effect(right.mode, right.colors[0], right.colors[1], right.colors[2], right.speed, right.palette, right.getOption(SEG_OPTION_ON));
        return true;
    }
    bool setRight(Effect rearSeg)
    {
        RightSegment = rearSeg;
        return true;
    }
    bool setRear()
    {
        if (!isSegmentInitialized(BRAKE_SEGMENT))
        {
            return false;
        }
        auto &rear = ST.rearSegment();
        RearSegment = Effect(rear.mode, rear.colors[0], rear.colors[1], rear.colors[2], rear.speed, rear.palette, rear.getOption(SEG_OPTION_ON));
        return true;
    }
    bool setRear(Effect rearSeg)
    {
        RearSegment = rearSeg;
        return true;
    }
    bool setFront()
    {
        if (!isSegmentInitialized(FRONT_SEGMENT))
        {
            return false;
        }
        auto &front = ST.frontSegment();
        FrontSegment = Effect(front.mode, front.colors[0], front.colors[1], front.colors[2], front.speed, front.palette, front.getOption(SEG_OPTION_ON));
        return true;
    }
    bool setFront(Effect frontSeg)
    {
        FrontSegment = frontSeg;
        return true;
    }
    void refresh()
    {
        setLeft();
        setRight();
        setRear();
        setFront();
    }

};

class EffectCacheCollection
{
public:
    EffectCache forDoor;
    EffectCache forBrake;
    EffectCache forLeftTurn;
    EffectCache forRightTurn;
    EffectCache forUnlock;
    EffectCache forReverse;
    EffectCache forLock;
    EffectCache forIgnition;
    EffectCache generic;

    EffectCacheCollection() : forDoor(), forBrake(), forLeftTurn(), forRightTurn(), forUnlock(), forReverse(), forLock(), forIgnition(), generic() {}
};