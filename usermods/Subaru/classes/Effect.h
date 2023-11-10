
#include "ColorPrint.h"
#include "SubaruTelemetry.h"

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
