#include "wled.h"
#include "SubaruTelemetry.h"

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
    uint8_t palette;
    bool power;

    Effect(uint8_t m, uint32_t c1, uint8_t s, uint8_t p, bool pw = true)
        : mode(m), colors{c1, 0x000000, 0x000000}, speed(s), palette(p), power(pw) {}

    Effect(uint8_t m, uint32_t c1, uint32_t c2, uint32_t c3, uint8_t s, uint8_t p, bool pw = true)
        : mode(m), colors{c1, c2, c3}, speed(s), palette(p), power(pw) {}
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

    EffectCollection() : doorOpen(Effect(FX_MODE_STATIC, 0xFFC68C, 255, 0)),
                         leftTurn(Effect(FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 0)),
                         rightTurn(Effect(FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 0)),
                         brake(Effect(FX_MODE_STATIC, 0xFF0000, 255, 0)),
                         reverse(Effect(FX_MODE_STATIC, 0xFFC68C, 255, 0)),
                         ignition(Effect(FX_MODE_LOADING, 0xFFC68C, 0x000000, 0x000000, 225, 0)),
                         unlock(Effect(FX_MODE_BLINK, 0xFFAA00, 200, 0)),
                         lock(Effect(FX_MODE_BLINK, 0xFF0000, 200, 0)) {}
};

class EffectCache
{
public:
    Effect LeftSegment;
    Effect RightSegment;
    Effect RearSegment;
    Effect FrontSegment;

    EffectCache() : LeftSegment(Effect(FX_MODE_STATIC, 0x000000, 255, 0)),
                    RightSegment(Effect(FX_MODE_STATIC, 0x000000, 255, 0)),
                    RearSegment(Effect(FX_MODE_STATIC, 0x000000, 255, 0)),
                    FrontSegment(Effect(FX_MODE_STATIC, 0x000000, 255, 0))
    {
        refresh();
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
        Segment leftSeg = strip.getSegments()[LEFT_SEGMENT];
        LeftSegment = Effect(leftSeg.mode, leftSeg.colors[0], leftSeg.colors[1], leftSeg.colors[2], leftSeg.speed, leftSeg.palette, leftSeg.getOption(SEG_OPTION_ON));
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
        Segment rightSeg = strip.getSegments()[RIGHT_SEGMENT];
        RightSegment = Effect(rightSeg.mode, rightSeg.colors[0], rightSeg.colors[1], rightSeg.colors[2], rightSeg.speed, rightSeg.palette, rightSeg.getOption(SEG_OPTION_ON));
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
        Segment rearSeg = strip.getSegments()[BRAKE_SEGMENT];
        RearSegment = Effect(rearSeg.mode, rearSeg.colors[0], rearSeg.colors[1], rearSeg.colors[2], rearSeg.speed, rearSeg.palette, rearSeg.getOption(SEG_OPTION_ON));
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
        Segment frontSeg = strip.getSegment(FRONT_SEGMENT);
        FrontSegment = Effect(frontSeg.mode, frontSeg.colors[0], frontSeg.colors[1], frontSeg.colors[2], frontSeg.speed, frontSeg.palette, frontSeg.getOption(SEG_OPTION_ON));
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

    EffectCacheCollection() : forDoor(), forBrake(), forLeftTurn(), forRightTurn(), forUnlock(), forReverse(), forLock(), forIgnition() {}
};