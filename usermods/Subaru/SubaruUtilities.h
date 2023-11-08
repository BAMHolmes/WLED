#include "wled.h"
#include "SubaruTelemetry.h"
#include <cstdint>
#include <array>
#include <unordered_map>
#include <map>
/**
 * A Class with methods for printing colored text to Serial monitor.
 * It accepts a String and a color code for the foreground and background color of the text .
 * The color codes should be enums like FG_RED, BG_GREEN for forground red and background green respectfully
 */
class ColorPrint
{
public:
    enum Color
    {
        FG_BLACK = 30,
        FG_RED = 31,
        FG_GREEN = 32,
        FG_YELLOW = 33,
        FG_BLUE = 34,
        FG_MAGENTA = 35,
        FG_CYAN = 36,
        FG_WHITE = 37,
        BG_BLACK = 40,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_YELLOW = 43,
        BG_BLUE = 44,
        BG_MAGENTA = 45,
        BG_CYAN = 46,
        BG_WHITE = 47
    };

    static void print(String text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK)
    {
        Serial.print("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
    }
    static void println(String text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK)
    {
        Serial.println("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
    }
};
ColorPrint p;
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
std::vector<int> DEFAULT_SEGMENT_IDS = {FRONT_SEGMENT, LEFT_SEGMENT, RIGHT_SEGMENT, REAR_SEGMENT};

class Effect
{
public:
    std::vector<int> segmentIDs;
    uint8_t mode;
    uint32_t colors[3];
    uint8_t speed;
    uint8_t fade;
    bool power;
    unsigned long startTime;
    bool isRunning;
    unsigned long runTime;
    unsigned long remainingRuntime;
    uint8_t palette;
    uint32_t checksum;

    Effect()
        : segmentIDs(DEFAULT_SEGMENT_IDS),
          mode(FX_MODE_STATIC),
          speed(255),
          fade(255),
          power(false),
          startTime(0),
          isRunning(false),
          runTime(0),
          remainingRuntime(0),
          palette(0),
          checksum(calculateChecksum()) // Ensure checksum is calculated last
    {
        std::fill(std::begin(colors), std::end(colors), 0x000000);
    }

    Effect(std::vector<int> seg, uint8_t m, uint32_t c1, uint8_t s, uint8_t f, unsigned long r, uint8_t p, bool pw = true)
        : segmentIDs(seg),
          mode(m),
          speed(s),
          fade(f),
          power(pw),
          startTime(0),
          isRunning(false),
          runTime(r),
          palette(p),
          checksum(calculateChecksum()) // Ensure checksum is calculated last
    {
        remainingRuntime = runTime;
        colors[0] = c1;
        colors[1] = 0x000000;
        colors[2] = 0x000000;
    }

    void start()
    {
        if (!isRunning)
        {
            triggerEffect(); // Actual function to start the effect
            isRunning = true;
        }
    }

    void stop()
    {
        // Code to stop the effect if necessary
        isRunning = false;
        p.println("Stopping effect " + String(checksum) + " on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
    }
    void triggerEffect()
    {
        // Iterate through all segmentIDs and print on a single line
        p.print("Triggering effect " + String(checksum) + " on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        for (const auto &segmentID : segmentIDs)
        {
            p.print(String(segmentID) + " ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        }
        p.println("...", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        /**
         *     int mode = effect.mode;
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
        */
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
    // segmentIDs, mode, colors{c1, 0x000000, 0x000000}, speed, fade, runTime, palette, power=on
    EffectCollection() : doorOpen(Effect(DEFAULT_SEGMENT_IDS, FX_MODE_STATIC, 0xFFC68C, 255, 255, 10000, 0)),
                         leftTurn(Effect({LEFT_SEGMENT}, FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 0, 3000, 0)),
                         rightTurn(Effect({RIGHT_SEGMENT}, FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 255, 3000, 0)),
                         brake(Effect({REAR_SEGMENT}, FX_MODE_STATIC, 0xFF0000, 255, 255, 0, 0)),
                         reverse(Effect({REAR_SEGMENT}, FX_MODE_STATIC, 0xFFC68C, 255, 255, 0, 0)),
                         ignition(Effect(DEFAULT_SEGMENT_IDS, FX_MODE_LOADING, 0xFFC68C, 225, 255, 3000, 0)),
                         unlock(Effect(DEFAULT_SEGMENT_IDS, FX_MODE_BLINK, 0xFFAA00, 200, 255, 3000, 0)),
                         lock(Effect(DEFAULT_SEGMENT_IDS, FX_MODE_BLINK, 0xFF0000, 200, 255, 3000, 0)),
                         off(Effect(DEFAULT_SEGMENT_IDS, FX_MODE_STATIC, 0x000000, 255, 255, 0, 0))
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

        printAllChecksums();
    }
    /** Print all effect names and their corresponding checksums */
    void printAllChecksums() const
    {
        Serial.println("*****Checksums for all effects:******");
        Serial.println("doorOpen: " + String(doorOpen.checksum));
        Serial.println("leftTurn: " + String(leftTurn.checksum));
        Serial.println("rightTurn: " + String(rightTurn.checksum));
        Serial.println("brake: " + String(brake.checksum));
        Serial.println("reverse: " + String(reverse.checksum));
        Serial.println("ignition: " + String(ignition.checksum));
        Serial.println("unlock: " + String(unlock.checksum));
        Serial.println("lock: " + String(lock.checksum));
        Serial.println("off: " + String(off.checksum));
        Serial.println("*************************************");
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
};

class QueueItem
{
public:
    Effect effect;            // The Effect to be run
    unsigned long runTime;    // Duration the Effect should run
    unsigned long transition; // Transition time for the Effect

    // The constructor now only needs to take runTime and transition, since Effect contains segmentIDs
    QueueItem(Effect effect, unsigned long transition)
        : effect(effect), transition(transition)
    {
        // Start time is set when the effect is triggered, not here in the constructor
    }

    bool isExpired(unsigned long currentTime) const
    {
        return currentTime - effect.startTime >= effect.runTime;
    }

    // Other QueueItem methods as necessary
};

class QueueManager
{
private:
    std::vector<Effect> effectsQueue;
    std::map<int, Effect *> activeEffectsPerSegment;

public:
    void addEffectToQueue(Effect effect)
    {
        unsigned long currentTime = millis();
        effect.startTime = currentTime; // Set the start time to now

        // Check for an existing effect with the same checksum to extend its runtime
        for (auto &queuedEffect : effectsQueue)
        {
            if (queuedEffect.checksum == effect.checksum)
            {
                // Increase the remaining runtime of the effect already in the queue
                queuedEffect.remainingRuntime += effect.runTime;
                Serial.println("Extended runtime of effect " + String(effect.checksum));
                return;
            }
        }

        // Add the new effect to the queue
        effectsQueue.push_back(effect);
        Serial.println("Added new effect " + String(effect.checksum) + " to queue with runtime " + String(effect.runTime));

        // Update the currently active effect for each affected segment
        for (int segmentID : effect.segmentIDs)
        {
            // If there's already an effect running on this segment, just mark it for update
            // without altering its run time; it will be updated in processQueue
            if (activeEffectsPerSegment.find(segmentID) != activeEffectsPerSegment.end())
            {
                Serial.println("Effect " + String(effect.checksum) + " will override segment " + String(segmentID) + " after current effect completes.");
            }
            // Set this effect as the next to be displayed on the segment
            activeEffectsPerSegment[segmentID] = &effect;
        }
    }

    void processQueue()
    {
        unsigned long currentTime = millis();

        // Iterate over the queue and update the state of each effect
        for (Effect &effect : effectsQueue)
        {
            if (currentTime - effect.startTime < effect.remainingRuntime)
            {
                // The effect is within its run time
                for (int segmentID : effect.segmentIDs)
                {
                    Effect *currentEffect = activeEffectsPerSegment[segmentID];
                    // If this effect is the most recent for the segment, start or continue it
                    if (&effect == currentEffect && !effect.isRunning)
                    {
                        effect.start();
                    }
                }
            }
            else
            {
                // The effect has exceeded its run time, stop if it's running
                if (effect.isRunning)
                {
                    effect.stop();
                }
            }
        }

        // Clean up the queue, removing effects that have finished running
        effectsQueue.erase(
            std::remove_if(
                effectsQueue.begin(),
                effectsQueue.end(),
                [currentTime](const Effect &effect)
                {
                    return currentTime - effect.startTime >= effect.remainingRuntime;
                }),
            effectsQueue.end());
    }
};
// emplace_back <- crazy method.

QueueManager queueManager;

/** Checksums
 *
 * doorOpen: 16763019
 * leftTurn: 16755493
 * rightTurn: 16755748
 * brake: 16712191
 * reverse: 16763019
 * ignition: 16763036
 * unlock: 16755657
 * lock: 16712137
 * off: 511
 *
 */