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
#include <functional>
#include <chrono>
#include <thread>
/**
 * @brief Class for managing the queue of effects for each segment.
 *
 */
class QueueManager
{

public:
    QueueManager()
    {
        effects.printAllChecksums();
        for (int segmentID : SUBARU_GROUND_SEGMENT_IDS)
        {
            segmentPowerStatus[segmentID] = false;
        }
    }

private:
    std::map<int, std::deque<Effect>> effectsPerSegment;
    std::map<int, Effect *> activeEffectsPerSegment;
    std::unordered_map<int, bool> segmentPowerStatus;
    static std::chrono::steady_clock::time_point powerOffTimer;
    unsigned long lastSegmentCheckTime = 0;
    const unsigned long powerOffDelay = 30000; // Delay in milliseconds (30 seconds)


public:
    /**
     * @brief Checks the LED segments and updates them if necessary.
     *
     */

    /**
     * Adds the given effect to the end of the effect queue.
     *
     * @param effect The effect to add to the queue.
     */
    void addEffectToQueue(Effect effect)
    {
        //Serial.println("addEffectToQueue(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        if (!effect.name){
            return;
        }
        //Serial.println("addEffectToQueue(): effect name is: " + String(effect.name));
        bool isPreset = effect.isPreset();
        unsigned long currentTime = millis();
        effect.startTime = currentTime;
        // This handles both custom and preset effects, and places them appropriately in the queue
        for (int segmentID : effect.segmentIDs)
        {
            //Serial.println("addEffectToQueue(): Segment ID: " + String(segmentID));
            auto &queue = effectsPerSegment[segmentID];
            bool queueIsEmpty = queue.empty();
            if (queueIsEmpty)
            {
                p.println("addEffectToQueue(): Queue on segment " + String(segmentID) + " is empty, adding effect...", ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
                queue.push_front(Effect(segmentID));
                queue.push_front(effect);
                continue;
            }
            // This is a preset effect, add it to the front of the queue
            // Loop through the queue and check if an effect with the same name and checksum already exists
            // if so, append the new effect's runtime to the existing effect's runtime and remove the new effect
            // if not, add the new effect to the front of the queue
            bool effectExists = false;

            for (Effect &existingEffect : queue)
            {
                //Print both checksums 
                //Serial.println("addEffectToQueue(): Comparing checksums: " + String(existingEffect.checksum) + " vs " + String(effect.checksum) + ".");
                if (existingEffect.checksum == effect.checksum)
                {
                    p.println("addEffectToQueue(): Effect [" + String(effect.name) + "] already exists in the queue on segment " + String(segmentID) + ", appending run time...", ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
                    existingEffect.startTime = currentTime;
                    effectExists = true;
                }
             

            }
            if(effectExists){
                continue;
            }
            if (!isPreset)
            {

                // This is a custom effect, place it at the beginning of the queue
                p.println("addEffectToQueue(): Adding custom effect [" + String(effect.name) + "] to the back of the queue on segment " + String(segmentID));
                queue.push_back(effect);
            }
            else
            {
                p.println("addEffectToQueue(): Adding preset [" + String(effect.name) + "] to the front of the queue on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);

                queue.push_front(effect);
            }

            //Iterate print the names of all effects within the queue.
            for (Effect &existingEffect : queue)
            {
                p.println("addEffectToQueue(): Effect in queue: " + String(existingEffect.name) + " on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_BLUE);
            }

        }
        
    }
    void checkSegmentsAndUpdate()
    {
    
        //Serial.println("checkSegmentsAndUpdate(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        if (strip.isUpdating())
        {
            //Serial.println("checkSegmentsAndUpdate(): Strip is updating, skipping segment check...");
            return;
        }
        unsigned long currentTime = millis();
        bool allSegmentsPoweredOff = true; // Assume initially that all segments are off

        // Iterate over each segment
        for (int segmentID : ALL_SUBARU_SEGMENT_IDS)
        {
            bool updatedNewEffect = false;
            Effect currentEffect = Effect(segmentID);
            //Serial.println("checkSegmentsAndUpdate(): Segment " + String(segmentID) + "is being processed. Current effect checksum: " + String(currentEffect.checksum) + ".");
            // Check if there's already an active effect for this segment
            if (activeEffectsPerSegment.find(segmentID) != activeEffectsPerSegment.end())
            {
                //Serial.println("checkSegmentsAndUpdate(): Active effect found in \"activeEffect[]\" for segment " + String(segmentID) + ".");
                Effect *activeEffect = activeEffectsPerSegment[segmentID];
                bool presetIsActive = activeEffect->isPreset();
                bool currentEffectChanged = activeEffect->checksum != currentEffect.checksum;
                bool OKtoUpdate = !presetIsActive && currentEffectChanged;
                //Serial.println("checkSegmentsAndUpdate(): " + String(presetIsActive ? "Preset" : "Custom") + " effect [" + String(activeEffect->name) + "] found in \"activeEffect[]\" for segment " + String(segmentID) + ". Ok to update? " + String(OKtoUpdate ? "YES" : "No") + ".");

                // Check if the active effect is a non-preset and if its properties differ from the current segment settings
                if (OKtoUpdate)
                {
                    //Serial.println("checkSegmentsAndUpdate(): Active effect is a custom effect and its properties have changed.");
                    // Effect properties have changed, replace the effect in the queue
                    effectsPerSegment[segmentID].pop_front();                                   // Remove the old effect from the queue
                    effectsPerSegment[segmentID].push_front(currentEffect);                     // Add the new effect to the front of the queue
                    activeEffectsPerSegment[segmentID] = &effectsPerSegment[segmentID].front(); // Update active effect pointer
                    updatedNewEffect = true;
                    //Serial.println("checkSegmentsAndUpdate(): Added custom effect to the queue for segment(" + String(segmentID) + ") with checksum: " + String(currentEffect.checksum) + ".");
                    //delay(5000);
                }
            }

            if (!updatedNewEffect)
            {
                // If no custom effect is found or the segment was empty, check if the segment should start a new effect
                bool effectsPerSegmentIsEmpty = effectsPerSegment[segmentID].empty();
                bool currentEffectIsDifferent = false;
                if(!effectsPerSegmentIsEmpty){
                    ////Serial.println("checkSegmentsAndUpdate(): Segment " + String(segmentID) + " has effects in the queue.");
                    auto effectAtFront = effectsPerSegment[segmentID].front();
                    currentEffectIsDifferent = effectAtFront.weakChecksum != currentEffect.weakChecksum;
                    if(currentEffectIsDifferent){
                        //Serial.println("checkSegmentsAndUpdate(): Current effect is different: " + String(effectAtFront.name) + " vs " + String(currentEffect.name)  + ".");
                        activeEffectsPerSegment[segmentID] = &effectAtFront;
                    }
                }
                // if(currentEffectIsDifferent || effectsPerSegmentIsEmpty){
                //     effectsPerSegment[segmentID].push_front(currentEffect);
                //     activeEffectsPerSegment[segmentID] = &currentEffect;
                //     //Serial.println("checkSegmentsAndUpdate(): Added effect to the queue for segment(" + String(segmentID) + ") with checksum: " + String(currentEffect.checksum) + ".");
                // }
            }

            // Determine if this segment is considered "powered on"
            if (currentEffect.power)
            {
                allSegmentsPoweredOff = false; // If any segment is powered on, set this flag to false
            }
        }

        // Manage the power relay based on the allSegmentsPoweredOff flag and the last time segments were powered
        if (allSegmentsPoweredOff)
        {
            //Serial.println("checkSegmentsAndUpdate(): All segments are off.");
            if (currentTime - lastSegmentCheckTime >= powerOffDelay)
            {
                //Serial.println("checkSegmentsAndUpdate(): All segments are off and the delay has passed.");
                ST->turnOffAllRelays(); // Turn off the relay if all segments are off and the delay has passed
                //SubaruTelemetry::getInstance()->turnOffGroundLEDRelay(); // Turn off the relay if all segments are off and the delay has passed
            }
        }
        lastSegmentCheckTime = currentTime;                      // Reset the timer

    }

    /**
     * Process the effect queue which should just be a stack of effects that are processed in order.
     * 1. Iterate through each effect in the queue
     * 1.1 If the effect is not a preset, continue
     * 2. Check if the effect is started, if not, start it
     * 3. Check if the effect has expired, if so, stop it and remove it from the queue
     * 4. If the queue is empty, do nothing and continue
     * 
     */
    void processQueue()
    {
        delay(3000);

        checkSegmentsAndUpdate();
        //Serial.println("processQueue(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        unsigned long currentTime = millis();
        // Iterate over each segment's queue
        for (auto &pair : effectsPerSegment)
        {
            //Serial.println("processQueue(): Segment " + String(pair.first) + "...");
            int segmentID = pair.first;
            std::deque<Effect> &queue = pair.second;
            // Check if the queue is empty or if the current effect is not a preset
            //if (queue.empty() || !isPresetEffect(queue.front()))
            if(queue.empty())
            {
                //p.println("processQueue(): Queue on segment " + String(segmentID) + " is empty, skipping...", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
                continue;
            }
            // Process the first effect in the queue if it's not already running
            Effect *currentEffect = &queue.front();
            bool triggering = false;
            try {
                triggering = currentEffect->checkTrigger();
            } catch (const std::exception& e) {
                //Serial.println("processQueue(): Exception caught in checkTrigger: " + String(e.what()));
                continue; // Skip this iteration or handle the error appropriately
            }            
            // p.println("Brake Status: " + String(triggering));
            bool active = triggering || (currentTime - currentEffect->startTime < currentEffect->runTime);
            bool expired = (currentTime - currentEffect->startTime >= currentEffect->runTime);

            // Check if the effect is a preset and if should be running. If so, start it
            //Serial.println("processQueue(): Checking effect [" + String(currentEffect->name) + "] on segment " + String(segmentID) + "...");
            if (active && !currentEffect->isRunning)
            {
                p.println("processQueue(): Starting preset effect [" + String(currentEffect->name) + "] on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
                activeEffectsPerSegment[segmentID] = &currentEffect->setPower(true);
                currentEffect->start();
            }
            else if (!expired && triggering)
            {
                p.println("processQueue(): TRIGGERED: Effect [" + String(currentEffect->name) + "] on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
            }
            else if (expired && !triggering)
            {
                p.println("processQueue(): [" + String(currentEffect->name) + "] on segment " + String(segmentID) + " has expired, stopping...", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
                // Move to the next effect if the current one has finished
                auto currentEffectCache = currentEffect;
                currentEffect->stop();
                queue.pop_front();
                // Remove the currentEffect from activeEffectsPerSegment
                activeEffectsPerSegment.erase(segmentID);

                // Print and handle the queue length and transitions
                if (queue.empty())
                {
                    p.println("processQueue(): Queue on segment " + String(segmentID) + " is empty, starting off effect...", ColorPrint::FG_WHITE, ColorPrint::BG_CYAN);
                    //delay(5000);
                    Effect offEffect = effects.off.setPower(false).setColor(0x000000);
                    offEffect.start(segmentID);
                }
                else
                {
                    //Serial.println("processQueue(): Stopping checksum: " + String(currentEffect->checksum) + ".");
                    currentEffect = &queue.front().setInterimTransitionSpeed(currentEffectCache->transitionSpeed);
                    if (!currentEffect->power)
                    {
                        //Serial.println("processQueue(): Current effect has no power??");
                        //delay(5000);
                        currentEffect->stop();
                        //currentEffect = &currentEffectCache->setName("Shut Down").setPower(false).setChecksum();
                        activeEffectsPerSegment.erase(segmentID);
                    }
                    else
                    {
                        activeEffectsPerSegment[segmentID] = currentEffect;
                    }
                    // p.println("Starting checksum: " + String(currentEffect->checksum), ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
                    // p.println("Starting next effect in queue (" + String(currentEffect->name) + ") on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_CYAN);

                    currentEffect->start(segmentID);
                }
            }
        }
    }
};

QueueManager queueManager;

// emplace_back <- crazy method.
