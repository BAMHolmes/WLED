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

typedef std::deque<Effect *> EffectsQueue;
typedef std::map<int, EffectsQueue *> EffectsPerSegment;
typedef std::map<int, Effect *> EffectPerSegmentShadow;
class QueueManager
{

public:
    QueueManager()
    {
        //Iterate through all segment ids and initialize generic effects in the shadow.
        for (int segmentID : ALL_SUBARU_SEGMENT_IDS)
        {
            shadowEffectPerSegment[segmentID] = effects.generateEffect(segmentID);
            shadowEffectPerSegment[segmentID]->morph();
        }

        effects.printAllChecksums();
        for (int segmentID : ALL_SUBARU_SEGMENT_IDS)
        {
            segmentPowerStatus[segmentID] = false;
        }
    }

private:
    enum Action
    {
        Starting,
        Stopping,
        Continuing
    };
    EffectsPerSegment effectsPerSegment;
    EffectPerSegmentShadow shadowEffectPerSegment;
    EffectsPerSegment activeEffectsPerSegment;

    std::unordered_map<int, bool> segmentPowerStatus;
    static std::chrono::steady_clock::time_point powerOffTimer;
    unsigned long lastSegmentCheckTime = 0;
    const unsigned long powerOffDelay = 30000; // Delay in milliseconds (30 seconds)
    EffectsQueue *getOrCreateQueue(int segmentID)
    {
        auto it = effectsPerSegment.find(segmentID);
        if (it != effectsPerSegment.end()) {
            return it->second;
        }

        // Create new queue for the segment
        EffectsQueue *segmentQueue = new EffectsQueue();
        effectsPerSegment[segmentID] = segmentQueue;
        return segmentQueue;
    }
public:
    void printQueue(int segmentID)
    {
        EffectsQueue *segmentQueue = getOrCreateQueue(segmentID);
        if (!segmentQueue) {
            //p.println("\tprintQueue Error. No segmentQueue defined", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
            return; // Check for null paointer
        }

        //p.print("\tprintQueue(" + String(segmentID) + "): Effects in queue: ", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        for (Effect *effect : *segmentQueue)
        {
            //p.print(effect->name + ", ", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        }
        //p.println("", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
    }
    void addEffectToQueue(int segmentID, Effect *effectToAdd)
    {
        if (effectToAdd == nullptr) return;
        
        //Print the length of the effectsPerSegment map
        EffectsQueue *segmentQueue = getOrCreateQueue(segmentID);
        if (!segmentQueue) return; // Check for null pointer

        //Loop through and print all effects in the queue


        // Check if Effect is already in the queue
        for (Effect *queuedEffect : *segmentQueue)
        {
            if (queuedEffect->sameAs(effectToAdd))
            {
                // Serial.println("addEffectToQueue(): Effect [" + String(effect.name) + "] already exists in the queue, appending run time...");
                //p.println("addEffectToQueue(" + String(segmentID) + "): Effect [" + String(effectToAdd->name) + "] already exists in the queue as (" + String(queuedEffect->name) + "), appending run time...", ColorPrint::FG_BLACK, ColorPrint::BG_YELLOW);
                queuedEffect->start(segmentID);
                return;
            }
        }
        // Get the effect that's currently active on the segment.
        Effect *effectOnSegment = shadowEffectPerSegment[segmentID];
        if (effectOnSegment == nullptr) {
            effectOnSegment = effects.generateEffect(segmentID); // Ensure there is a fallback or handle this scenario appropriately
        }
        effectOnSegment->morph();
        bool effectOnSegmentIsPreset = effectOnSegment->isPreset();
        bool effectOnSegmentSameAsEffectToAdd = effectOnSegment->sameAs(effectToAdd);
        bool segmentQueueIsEmpty = segmentQueue->empty();
        bool effectToAddAlreadyQueued = !segmentQueueIsEmpty && segmentQueue->front()->sameAs(effectToAdd);
        bool effectAlreadyQueued = effectOnSegmentSameAsEffectToAdd || effectToAddAlreadyQueued;
        //Check if the back of the segmentQueue has an effect

        // Print current state of affairs
        //p.println("addEffectToQueue(" + String(segmentID) + "): Current effect on segment " + String(segmentID) + ": " + String(effectOnSegment->name), ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        //p.print("\tEffect to add: ", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        //p.println(effectToAdd->name, ColorPrint::FG_WHITE, ColorPrint::BG_GREEN);
        //p.println("\tEffect on segment is preset: " + String(effectOnSegmentIsPreset), ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        //p.println("\tEffect on segment is same as effect to add: " + String(effectOnSegmentSameAsEffectToAdd), ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        if (effectAlreadyQueued)
        {
            // Serial.println("addEffectToQueue(): Effect [" + String(effect.name) + "] is already active on the segment, skipping...");
            //p.println("\tEffect [" + String(effectToAdd->name) + "] is already active on the segment, skipping...", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
            return;
        }
        bool segmentQueueHasEffectOnSegment = !segmentQueueIsEmpty && segmentQueue->back()->sameAs(effectOnSegment);
        if (!segmentQueueHasEffectOnSegment && !effectOnSegmentIsPreset)
        {
            bool shouldReplaceBack = !segmentQueueIsEmpty && !segmentQueue->back()->isPreset();
            if(shouldReplaceBack){
                //p.println("\tRunning checksum [ " + segmentQueue->front()->weakChecksum + " ]", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);

                ////p.println("\tEffect [" + String(effectOnSegment->name) + "] is not a preset, replacing it with [" + String(effectToAdd->name) + "]...", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
                segmentQueue->pop_back();
            }
            effectOnSegment->isPreset(true);
            //p.println("\tEffect [" + String(effectOnSegment->name) + "] is DEFINITELY not a preset ( " + String(effectOnSegment->weakChecksum) + " ), pushing it to the back...", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
            segmentQueue->push_back(new Effect(*effectOnSegment));
        }

        //p.println("\tAdding effect [" + String(effectToAdd->name) + "] to the queue...", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
        segmentQueue->push_front(effectToAdd);
        
    }

    // Method to clear effectsPerSegment
    void clearEffectsPerSegment()
    {
        // Loop through all items held within effectsPerSegment and clear
        for (auto &pair : effectsPerSegment)
        {
            if (pair.second)
            {
                pair.second->clear();
            }
        }
        effectsPerSegment.clear();
    }


    void processQueue(int segmentID)
    {

        ////p.println("processQueue(" + String(segmentID) + "): Processing queue for segment " + String(segmentID), ColorPrint::FG_BLACK, ColorPrint::BG_GRAY);
        printQueue(segmentID);
        // Check if the queue is empty
        EffectsQueue *segmentQueue = getOrCreateQueue(segmentID);
        bool isUpdating = strip.isUpdating();
        if (isUpdating || !segmentQueue) return; // Check for null pointer

        bool segmentQueueIsEmpty = segmentQueue->empty();
        if (segmentQueueIsEmpty)
        {
            //p.println("\tQueue on segment " + String(segmentID) + " is empty, skipping...", ColorPrint::FG_BLACK, ColorPrint::BG_GRAY);
            return;
        }
        // Hold a reference to the current effect
        Effect *effectToRun = segmentQueue->front();
        // Hold a reference to the actual effect that's currently running on the segment
        Effect *effectOnSegment = shadowEffectPerSegment[segmentID];

        effectOnSegment->morph();

        //printQueue(segmentID);
        bool effectMismatch = !effectToRun->sameAs(effectOnSegment);
        bool useSegmentEffect = !effectToRun->isPreset() && effectMismatch;
        if(useSegmentEffect){
            //p.println("\tEffect at front [" + String(effectToRun->name) + "] is not a preset, using segment effect [" + String(effectOnSegment->name) + "]...", ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
            effectToRun = effectOnSegment;
        }

        bool segmentQueueHasEffectOnSegment = segmentQueue->back()->sameAs(effectOnSegment);
  
        bool expired = effectToRun->isExpired(segmentID);
        if (expired)
        {
            //p.println("\tEffect [" + String(effectToRun->name) + "] expired. Stopping...", ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
            segmentQueue->pop_front();

            if(segmentQueue->empty()){
               //p.println("\t" + String(effectToRun->name) + " expired and the queue is empty. Stopping and starting " + String(effects.off.name) + ".", ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
                delay(5000);

               effectToRun->stop(segmentID, &effects.off);
            }else{
                auto *newFrontEffect = segmentQueue->front();
                //p.println("\t" + String(effectToRun->name) + " expired. Stopping and starting: " + String(newFrontEffect->name), ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
                effectToRun->stop(segmentID, newFrontEffect);
            }
            return;
        }
        if (effectMismatch  && !segmentQueueHasEffectOnSegment && !effectOnSegment->isPreset(true))
        {
            bool shouldReplaceBack = !segmentQueue->back()->isPreset();
            if(shouldReplaceBack){
                ////p.println("\tEffect [" + String(effectOnSegment->name) + "] is not a preset, replacing it with [" + String(effectToAdd->name) + "]...", ColorPrint::FG_BLACK, ColorPrint::BG_WHITE);
                segmentQueue->pop_back();
            }
            //p.println("\tEffect [" + String(effectOnSegment->name) + "] is not a preset, pushing it to the back...", ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
            segmentQueue->push_back(new Effect(*effectOnSegment));
        }        
        if (effectMismatch)
        {
            //p.println("\tEffect [" + String(effectToRun->name) + "] isn't on the segment (segment has [" + String(effectOnSegment->name) + "]). Triggering...", ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
            effectToRun->triggerEffect(segmentID);
        }
        //p.println("\tContinuing effect [" + String(effectToRun->name) + "] on segment " + String(segmentID), ColorPrint::FG_WHITE, ColorPrint::BG_MAGENTA);
      
    }
};

QueueManager queueManager;
void SegCon::resetAnyEffects()
{
    //p.println("resetAnyEffects(): Clearing effectsPerSegment...", ColorPrint::FG_WHITE, ColorPrint::BG_RED);
    queueManager.clearEffectsPerSegment();
}
// emplace_back <- crazy method.
