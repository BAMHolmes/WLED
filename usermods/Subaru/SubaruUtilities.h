#include "wled.h"
#include "SubaruTelemetry.h"
#include <cstdint>
#include <array>
#include <unordered_map>
#include <map>
#include <deque>
#include <vector>

/**
 * A Class with methods for printing colored text to Serial monitor.
 * It accepts a String and a color code for the foreground and background color of the text .
 * The color codes should be enums like FG_RED, BG_GREEN for forground red and background green respectfully
 */
class ColorPrint
{
public:
    static bool enabled;
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
        BG_WHITE = 47,
        BG_GRAY = 100

    };

    static void print(String text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK)
    {
        if(!enabled) return;
        Serial.print("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
    }
    static void println(String text, Color fgColor = FG_WHITE, Color bgColor = BG_BLACK)
    {
        if(!enabled) return;
        Serial.println("\033[" + String(fgColor) + ";" + String(bgColor) + "m" + text + "\033[0m");
    }
};
// Disable or enable color print messages
bool ColorPrint::enabled = true;


ColorPrint p;
class EffectCollection;
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
    String name;
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
    String checksum;

    Effect()
        : name("Default"),
          segmentIDs(DEFAULT_SEGMENT_IDS),
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

    Effect(String n, std::vector<int> seg, uint8_t m, uint32_t c1, uint8_t s, uint8_t f, unsigned long r, uint8_t p, bool pw = true)
        : name(n),
          segmentIDs(seg),
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
    /** Construct an effect from a Segment id. Use ST to fetch the segment by ID */
    Effect(int seg);

    void start()
    {
        if (!isRunning && segmentIDs.size() > 0)
        {
            triggerEffect(); // Actual function to start the effect
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
        p.println(".");
    }
    void triggerEffect()
    {
        p.print("Triggering [" + name + "]  on segments: ", ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        for (const auto &segmentID : segmentIDs)
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

            SubaruTelemetry::seg(segmentID).setOption(SEG_OPTION_ON, on);
            SubaruTelemetry::seg(segmentID).fade_out(fade);
            SubaruTelemetry::seg(segmentID).setColor(0, color1);
            SubaruTelemetry::seg(segmentID).setColor(1, color2);
            SubaruTelemetry::seg(segmentID).setColor(2, color3);
            SubaruTelemetry::seg(segmentID).setPalette(palette);
            SubaruTelemetry::seg(segmentID).speed = speed;

            strip.setMode(segmentID, mode);
            strip.setBrightness(255, true);
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
        String checksum = String(mode);
        checksum += ":" + String(colors[0]);
        checksum += ":" + String(speed);
        checksum += ":" + String(palette);
        checksum += ":" + String(power);
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
    // segmentIDs, mode, colors{c1, 0x000000, 0x000000}, speed, fade, runTime, palette, power=on
    EffectCollection() : doorOpen(Effect("Door Open", DEFAULT_SEGMENT_IDS, FX_MODE_STATIC, 0xFFC68C, 255, 255, 10000, 0)),
                         leftTurn(Effect("Left Turn", {LEFT_SEGMENT}, FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 0, 3000, 0)),
                         rightTurn(Effect("Right Turn", {RIGHT_SEGMENT}, FX_MODE_RUNNING_COLOR, 0xFFAA00, 255, 255, 3000, 0)),
                         brake(Effect("Brake Engaged", {REAR_SEGMENT}, FX_MODE_STATIC, 0xFF0000, 255, 255, 0, 0)),
                         reverse(Effect("Reverse Engaged", {REAR_SEGMENT}, FX_MODE_STATIC, 0xFFC68C, 255, 255, 0, 0)),
                         ignition(Effect("Ignition On", DEFAULT_SEGMENT_IDS, FX_MODE_LOADING, 0xFFC68C, 225, 255, 3000, 0)),
                         unlock(Effect("Car Unlocked", DEFAULT_SEGMENT_IDS, FX_MODE_BLINK, 0xFFAA00, 200, 255, 3000, 0)),
                         lock(Effect("Car Locked", DEFAULT_SEGMENT_IDS, FX_MODE_BLINK, 0xFF0000, 200, 255, 3000, 0)),
                         off(Effect("Segment Off", DEFAULT_SEGMENT_IDS, FX_MODE_STATIC, 0x000000, 255, 255, 0, 0))
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
};

EffectCollection effects;

Effect::Effect(int seg)
{
    name = String("EFS" + String(seg));
    Segment &segment = SubaruTelemetry::seg(seg);
    segmentIDs.push_back(seg);
    mode = segment.mode;
    speed = segment.speed;
    fade = 255;
    power = segment.getOption(SEG_OPTION_ON);
    startTime = 0;
    isRunning = false;
    runTime = 0;
    palette = segment.palette;
    colors[0] = segment.colors[0];
    colors[1] = segment.colors[1];
    colors[2] = segment.colors[2];
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
/**
 * @brief Class for managing the queue of effects for each segment.
 * 
 */
class QueueManager
{
private:
    std::map<int, std::deque<Effect>> effectsPerSegment;
    std::map<int, Effect *> activeEffectsPerSegment;

    // Check if the effect is a predefined one
    bool isPresetEffect(const Effect &effect) const
    {
        return effects.isPreset(&effect);
    }
    // Check if the effect is a predefined one when the effect is a pointer
    bool isPresetEffect(const Effect *effect) const
    {
        return effects.isPreset(effect);
    }

public:
    /**
     * @brief Checks the LED segments and updates them if necessary.
     * 
     */
    void checkSegmentsAndUpdate()
    {
        // Check each segment and if a custom effect is found that is not in the queue, enqueue it
        for (int segmentID : DEFAULT_SEGMENT_IDS)
        {
            Effect currentEffectOnSegment = Effect(segmentID);
            if (!isPresetEffect(currentEffectOnSegment))
            {
                // p.println("Checking segment " + String(segmentID) + " for custom effects...", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
                auto &queue = effectsPerSegment[segmentID];
                if (queue.empty())
                {
                    p.println("Adding custom effect [" + String(currentEffectOnSegment.name) + "] to the back of the queue on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
                    queue.push_back(currentEffectOnSegment);
                }
                else if (!strip.isUpdating() && queue.back().checksum != currentEffectOnSegment.checksum)
                {
                    p.println("Current seg(" + String(segmentID) + ") checksum: " + String(currentEffectOnSegment.checksum) + ", last seg(" + String(segmentID) + ") checksum: " + String(queue.back().checksum), ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
                    // p.println("Replacing custom effect [" + String(currentEffectOnSegment.name) + "] in the queue on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
                    queue.pop_back();
                    queue.push_back(currentEffectOnSegment);
                }
            }
            else
            {
                p.println("Segment " + String(segmentID) + " is running a preset effect:" + String(currentEffectOnSegment.name), ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
            }
        }
    }

    /**
     * Adds the given effect to the end of the effect queue.
     *
     * @param effect The effect to add to the queue.
     */
    void addEffectToQueue(Effect effect)
    {
        if (!effect.name)
            return;
        bool isPreset = isPresetEffect(effect);
        unsigned long currentTime = millis();
        effect.startTime = currentTime;
        // This handles both custom and preset effects, and places them appropriately in the queue
        for (int segmentID : effect.segmentIDs)
        {
            auto &queue = effectsPerSegment[segmentID];

            // This is a preset effect, add it to the front of the queue
            // Loop through the queue and check if an effect with the same name and checksum already exists
            // if so, append the new effect's runtime to the existing effect's runtime and remove the new effect
            // if not, add the new effect to the front of the queue
            bool effectExists = false;
            for (Effect &existingEffect : queue)
            {
                if (existingEffect.checksum == effect.checksum)
                {
                    p.println("Effect [" + String(effect.name) + "] already exists in the queue on segment " + String(segmentID) + ", appending run time...", ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
                    existingEffect.startTime = currentTime;
                    effectExists = true;
                    break;
                }
            }
            if (!effectExists && !isPreset)
            {

                // This is a custom effect, place it at the beginning of the queue
                p.println("Adding custom effect [" + String(effect.name) + "] to the back of the queue on segment " + String(segmentID));
                queue.push_back(effect);
            }
            else if (!effectExists && isPreset)
            {
                p.println("Adding preset [" + String(effect.name) + "] to the front of the queue on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);

                queue.push_front(effect);
            }
        }
    }

    /**
     * Process the effect queue for each segment and start the first effect in the queue if it's not already running.
     * If the effect is a preset and should be running, start it. If the effect has expired, stop it and start the next effect in the queue.
     */
    void processQueue()
    {
        checkSegmentsAndUpdate();
        unsigned long currentTime = millis();

        // Iterate over each segment's queue
        for (auto &pair : effectsPerSegment)
        {
            int segmentID = pair.first;
            std::deque<Effect> &queue = pair.second;

            if (queue.empty())
                continue;

            // Process the first effect in the queue if it's not already running
            Effect *currentEffect = &queue.front();
            bool isPreset = isPresetEffect(currentEffect);
            bool active = (currentTime - currentEffect->startTime < currentEffect->runTime);
            bool expired = (currentTime - currentEffect->startTime >= currentEffect->runTime);

            // Check if the effect is a preset and if should be running. If so, start it
            if (isPreset && active && !currentEffect->isRunning)
            {
                p.println("Starting preset effect [" + String(currentEffect->name) + "] on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
                activeEffectsPerSegment[segmentID] = currentEffect;
                currentEffect->start();
            }
            else if (isPreset && expired)
            {
                p.println("[" + String(currentEffect->name) + "] on segment " + String(segmentID) + " has expired, stopping...", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
                // Move to the next effect if the current one has finished
                currentEffect->stop();
                queue.pop_front();
                // Remove the currentEffect from activeEffectsPerSegment
                activeEffectsPerSegment.erase(segmentID);

                // Print and handle the queue length and transitions
                if (queue.empty())
                {
                    p.println("Queue on segment " + String(segmentID) + " is empty, starting off effect...", ColorPrint::FG_WHITE, ColorPrint::BG_CYAN);
                    Effect offEffect = effects.off;
                    offEffect.start();
                }
                else
                {
                    currentEffect = &queue.front();
                    p.println("Starting next effect in queue (" + String(currentEffect->name) + ") on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_CYAN);

                    currentEffect->start();
                    activeEffectsPerSegment[segmentID] = currentEffect;
                }
            }
        }
    }
};

QueueManager queueManager;

// emplace_back <- crazy method.
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