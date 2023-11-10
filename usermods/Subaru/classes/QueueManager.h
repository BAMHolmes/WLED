#include "wled.h"
#include "SubaruTelemetry.h"
#include <cstdint>
#include <array>
#include <unordered_map>
#include <map>
#include <deque>
#include <vector>
#include "Effect.h"
#include "ColorPrint.h"
/**
 * A Class with methods for printing colored text to Serial monitor.
 * It accepts a String and a color code for the foreground and background color of the text .
 * The color codes should be enums like FG_RED, BG_GREEN for forground red and background green respectfully
 */



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