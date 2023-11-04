#include "wled.h"
#include <FastLED.h>
#include <const.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define PCF8575_ADDRESS 0x20

// #define BRAKE_PEDAL 19
// #define DOOR_OPEN 5
// #define LEFT_INDICATOR 23
// #define RIGHT_INDICATOR 18
#define BRAKE_SEGMENT 0
#define LEFT_SEGMENT 1
#define FRONT_SEGMENT 2
#define RIGHT_SEGMENT 3

#define RIGHT_SEGMENT_START 0
#define RIGHT_SEGMENT_END 200
#define FRONT_SEGMENT_START 200
#define FRONT_SEGMENT_END 240
#define LEFT_SEGMENT_START 240
#define LEFT_SEGMENT_END 440
#define BRAKE_SEGMENT_START 440
#define BRAKE_SEGMENT_END 480


class Subaru : public Usermod {
  private:
    bool paused = false;
    bool flashingRed = false;
    bool welcomeLights = false;
    bool left_indicator_on = false;
    bool right_indicator_on = false;
    bool brake_pedal_pressed = false;
    bool left_indicator_previous_state = false;
    bool right_indicator_previous_state = false;
    bool brake_pedal_previous_state = false;
    bool reverse_previous_state = false;
    bool door_previous_state = false;
    bool override = false;
    bool door_is_open = false;
    bool car_in_reverse = false;
    bool EXT_PIN_1, EXT_PIN_2, EXT_PIN_3, EXT_PIN_4, EXT_PIN_5, EXT_PIN_6, EXT_PIN_7, EXT_PIN_8,
        EXT_PIN_9, EXT_PIN_10, EXT_PIN_11, EXT_PIN_12, EXT_PIN_13, EXT_PIN_14, EXT_PIN_15, EXT_PIN_16;

    const int NUM_PINS = 16;


    unsigned long lastTime = 0;
    unsigned long currentTime = 0;
    unsigned long period = 10000;

    uint8_t previousBrightness;
    uint8_t previousEffect;
    uint8_t previousPalette;
    
    uint8_t preBrakeBrightness;
    uint8_t preDoorBrightness;
    uint8_t preRightBrightness;
    uint8_t preLeftBrightness;
    uint8_t preFrontBrightness;

    //Declare Segments
    Segment brake_segment_cache;
 
    Segment left_segment_cache;

    Segment right_segment_cache;

    Segment front_segment_cache;


    Segment door_brake_segment_cache;
 
    Segment door_left_segment_cache;

    Segment door_right_segment_cache;

    Segment door_front_segment_cache;

    Segment brake_segment;
 
    Segment left_segment;

    Segment right_segment;

    Segment front_segment;
    
    struct PreviousState {
      uint8_t mode;
      uint8_t palette;
      uint8_t intensity;
      uint32_t * colors;
      uint8_t speed;
      uint8_t opacity;
      uint8_t cct;
    } previousLeftState, previousRightState, previousBrakeState, previousFrontState;
    /**
     * Loop through previousLeftState, previousRightState, previousBrakeState, previousFrontState and initialize them with empty values
    */


    /**
     * Create a new LED strip with 240 LEDs and a output pin of 3
    */


    void updateSegment(int seg, int mode, uint32_t color1, uint32_t color2, uint32_t color3, int speed, uint8_t palette = 0, bool on = true) {
      /** Return if seg is not defined */

      strip.setMode(seg, mode);
      strip.setBrightness(255, true);
      strip.getSegments()[seg].setOption(SEG_OPTION_ON, on);
      if(!on){
        color1 = 0x000000;
        color2 = 0x000000;
        color3 = 0x000000;
      }
      strip.getSegments()[seg].setColor(0, color1);
      strip.getSegments()[seg].setColor(1, color2);
      strip.getSegments()[seg].setColor(2, color3);
      strip.getSegments()[seg].setPalette(palette);

      strip.getSegments()[seg].speed = speed;

   
      strip.trigger();
    
    }

    void restorePreviousState(int segment, const Segment& prevState) {
      updateSegment(segment, prevState.mode, prevState.colors[0], prevState.colors[1], prevState.colors[2], prevState.speed, prevState.palette, prevState.on);
    }
    void select(int segment){
      right_segment.setOption(SEG_OPTION_SELECTED, 0);
      left_segment.setOption(SEG_OPTION_SELECTED, 0);
      brake_segment.setOption(SEG_OPTION_SELECTED, 0);
      front_segment.setOption(SEG_OPTION_SELECTED, 0);
      strip.getSegment(segment).setOption(SEG_OPTION_SELECTED, 1);
    }

    void selectAll(){
      right_segment.setOption(SEG_OPTION_SELECTED, 1);
      left_segment.setOption(SEG_OPTION_SELECTED, 1);
      brake_segment.setOption(SEG_OPTION_SELECTED, 1);
      front_segment.setOption(SEG_OPTION_SELECTED, 1);
    }

     void setupSegments(){
    
      strip.setSegment(BRAKE_SEGMENT, BRAKE_SEGMENT_START, BRAKE_SEGMENT_END, 1, 0, 0);
      strip.setSegment(LEFT_SEGMENT, LEFT_SEGMENT_START, LEFT_SEGMENT_END, 1, 0, 0);
      strip.setSegment(FRONT_SEGMENT, FRONT_SEGMENT_START, FRONT_SEGMENT_END, 1, 0, 0);
      strip.setSegment(RIGHT_SEGMENT, RIGHT_SEGMENT_START, RIGHT_SEGMENT_END, 1, 0, 0);
      
      right_segment = right_segment_cache = door_right_segment_cache = strip.getSegment(RIGHT_SEGMENT);
      left_segment = left_segment_cache = door_left_segment_cache = strip.getSegment(LEFT_SEGMENT);
      brake_segment = brake_segment_cache = door_brake_segment_cache = strip.getSegment(BRAKE_SEGMENT);
      front_segment = front_segment_cache = door_front_segment_cache = strip.getSegment(FRONT_SEGMENT);


      right_segment.setOption(SEG_OPTION_ON, 1);
      right_segment.setOption(SEG_OPTION_SELECTED, 1);
      left_segment.setOption(SEG_OPTION_ON, 1);
      left_segment.setOption(SEG_OPTION_SELECTED, 1);
      brake_segment.setOption(SEG_OPTION_ON, 1);
      brake_segment.setOption(SEG_OPTION_SELECTED, 1);
      front_segment.setOption(SEG_OPTION_ON, 1);
      front_segment.setOption(SEG_OPTION_SELECTED, 1);

      /** Set the name/title of segments */
      right_segment.name = (char*)"Right Strip";
      left_segment.name = (char*)"Left Strip";
      brake_segment.name = (char*)"Brake Strip";
      front_segment.name = (char*)"Front Strip";

    }

    unsigned long lastLeftIndicatorTime = 0;
    uint16_t currentLeftIndicatorIndex = 0;
    bool fadeLeftIndicatorDirection = true; // true for fading in, false for fading out
    byte currentLeftIndicatorBrightness = 0;
    uint16_t waitLeftIndicatorTime = 50; // time to wait between updates in ms
    uint16_t fadeLeftIndicatorAmount = 5; // amount to change brightness each update
    uint8_t indicator_palette[16] = {0, 0, 255, 0, 255, 165, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0};
    
    bool brake_effect_override;
    unsigned long last_brake_press_time = 0;
    unsigned long brake_effect_duration = 0;

    bool left_effect_override;
    unsigned long last_left_on_time = 0;
    unsigned long left_effect_duration = 5000;

    bool right_effect_override;
    unsigned long last_right_on_time = 0;
    unsigned long right_effect_duration = 5000;

    bool door_effect_override;
    unsigned long last_door_open_time = 0;
    unsigned long door_effect_duration = 10000;
 

  public:
    void setup() {
      // pinMode(LEFT_INDICATOR, INPUT_PULLDOWN);
      // pinMode(RIGHT_INDICATOR, INPUT_PULLDOWN);
      // pinMode(BRAKE_PEDAL, INPUT_PULLDOWN);
      // pinMode(DOOR_OPEN, INPUT_PULLDOWN);
      setupSegments();
      
      Wire.begin(SDA_PIN,SCL_PIN);

      writePCF8575(0x0000);   // Configure all pins as outputs initially
 
      Serial.println("All outputs written to PCF8575");

    }
    /**
     * A method that checks if each segment is assigned to the correct LED start and end. If a change/discrepancy is detected, run setup()
    */
    void checkSegmentIntegrity() {
        static bool previousIntegrityCheckResult = true;

        bool integrityCheckResult = true;

        // Check if each segment is assigned to the correct LED start and end
        integrityCheckResult &= strip.getSegment(BRAKE_SEGMENT).start == BRAKE_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(BRAKE_SEGMENT).stop == BRAKE_SEGMENT_END;
        integrityCheckResult &= strip.getSegment(LEFT_SEGMENT).start == LEFT_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(LEFT_SEGMENT).stop == LEFT_SEGMENT_END;
        integrityCheckResult &= strip.getSegment(FRONT_SEGMENT).start == FRONT_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(FRONT_SEGMENT).stop == FRONT_SEGMENT_END;
        integrityCheckResult &= strip.getSegment(RIGHT_SEGMENT).start == RIGHT_SEGMENT_START;
        integrityCheckResult &= strip.getSegment(RIGHT_SEGMENT).stop == RIGHT_SEGMENT_END;

        if (!integrityCheckResult && previousIntegrityCheckResult) {
            Serial.println("Detected a change in segment integrity. Running setup() again.");
            setupSegments();
        }

        previousIntegrityCheckResult = integrityCheckResult;
    }
    void printSegmentDetails(){

      /** Assign strip.getSegment(LEFT_SEGMENT) to a constant **/
      const Segment& left_segment = strip.getSegment(LEFT_SEGMENT);
      // const Segment& right_segment = strip.getSegment(RIGHT_SEGMENT);
      // const Segment& brake_segment = strip.getSegment(BRAKE_SEGMENT);

      /**
       * Print the current palette and mode of each segment every 5 seconds
       * */
      
      // Serial.println("------");
      // Serial.print("Brake Segment Mode: ");
      // Serial.println(brake_segment.mode);
      // Serial.print("Brake Segment Palette: ");
      // Serial.println(brake_segment.palette);
      // Serial.print("Brake Segment Intensity: ");
      // Serial.println(brake_segment.intensity);
      // Serial.print("Brake Segment Speed: ");
      // Serial.println(brake_segment.speed);
      // Serial.print("Brake Segment Opacity: ");
      // Serial.println(brake_segment.opacity);
      // Serial.print("Brake Segment CCT: ");
      // Serial.println(brake_segment.cct);
      // Serial.print("Brake Segment Color 1: ");
      // Serial.println(brake_segment.colors[0]);
      // Serial.print("Brake Segment Color 2: ");
      // Serial.println(brake_segment.colors[1]);
      // Serial.print("Brake Segment Color 3: ");
      // Serial.println(brake_segment.colors[2]);

      /**
       * Check if the previousLeftState is set.
       * If it is, print the previous state of the segment
       * */

      
      Serial.println("++++++ LEFT SEGMENT PREVIOUS COLORS ++++++");
      Serial.print("LSEG Prev Palette: ");
      Serial.println(left_segment_cache.palette);
      Serial.print("LSEG Prev Color 1: ");
      Serial.println(left_segment_cache.colors[0]);
      Serial.print("LSEG Prev Color 2: ");
      Serial.println(left_segment_cache.colors[1]);
      Serial.print("LSEG Prev Color 3: ");
      Serial.println(left_segment_cache.colors[2]);
    

 

      Serial.println("++++++ LEFT SEGMENT CURRENT COLORS ++++++");
      Serial.print("Left Segment Mode: ");
      Serial.println(left_segment.mode);
      Serial.print("Left Segment Palette: ");
      Serial.println(left_segment.palette);
      Serial.print("Left Segment Color 1: ");
      Serial.println(left_segment.colors[0]);
      Serial.print("Left Segment Color 2: ");
      Serial.println(left_segment.colors[1]);
      Serial.print("Left Segment Color 3: ");
      Serial.println(left_segment.colors[2]);
      // Serial.println("------");
      // Serial.print("Right Segment Mode: ");
      // Serial.println(right_segment.mode);
      // Serial.print("Right Segment Palette: ");
      // Serial.println(right_segment.palette);
      // Serial.print("Right Segment Intensity: ");
      // Serial.println(right_segment.intensity);
      // Serial.print("Right Segment Speed: ");
      // Serial.println(right_segment.speed);
      // Serial.print("Right Segment Opacity: ");
      // Serial.println(right_segment.opacity);
      // Serial.print("Right Segment CCT: ");
      // Serial.println(right_segment.cct);
      // Serial.print("Right Segment Color 1: ");
      // Serial.println(right_segment.colors[0]);
      // Serial.print("Right Segment Color 2: ");
      // Serial.println(right_segment.colors[1]);
      // Serial.print("Right Segment Color 3: ");
      // Serial.println(right_segment.colors[2]);
      // Serial.println("------");

    }
    void printDetailsPeriodically() {
        static unsigned long lastPrintTime = 0;
        //static bool previousStates[60] = {false}; // initialize all to LOW
        const unsigned long printInterval = 5000; // 5 seconds in milliseconds
        if (millis() - lastPrintTime >= printInterval) {
            printSegmentDetails();
            lastPrintTime = millis();
        }
    }
    void writePCF8575(uint16_t data) {
      Wire.beginTransmission(PCF8575_ADDRESS);  // Begin transmission to PCF8575
      Wire.write(data & 0xFF);              // Write low byte
      Wire.write(data >> 8);                // Write high byte
      Wire.endTransmission();               // End transmission
    }

    uint16_t readPCF8575() {
      uint16_t data = 0;
      Wire.beginTransmission(PCF8575_ADDRESS);  // Begin transmission to PCF8575
      Wire.endTransmission();  // End transmission

      Wire.requestFrom(PCF8575_ADDRESS, 2);  // Request 2 bytes from PCF8575
      if (Wire.available()) {
        data = Wire.read();           // Read the low byte
        data |= Wire.read() << 8;     // Read the high byte and shift it left
      }

      return data;
    }

    void loop() {
      static bool left_previously_set = false;
      static bool door_previously_set = false;
      static bool right_previously_set = false;
      static bool brake_previously_set = false;



      /** 
       * Loop through 16 pins on the PCF8575 expansion board and print the state to console on a single line separated by commas
      */

      uint16_t pinState = readPCF8575();  // Read the state of all pins
      
      // Assign the state of each pin
      EXT_PIN_1 = (pinState & 0x0001) != 0;
      EXT_PIN_2 = (pinState & 0x0002) != 0;
      EXT_PIN_3 = (pinState & 0x0004) != 0;
      EXT_PIN_4 = (pinState & 0x0008) != 0;
      EXT_PIN_5 = (pinState & 0x0010) != 0;
      EXT_PIN_6 = (pinState & 0x0020) != 0;
      EXT_PIN_7 = (pinState & 0x0040) != 0;
      EXT_PIN_8 = (pinState & 0x0080) != 0;
      EXT_PIN_9 = (pinState & 0x0100) != 0;
      EXT_PIN_10 = (pinState & 0x0200) != 0;
      EXT_PIN_11 = (pinState & 0x0400) != 0;
      EXT_PIN_12 = (pinState & 0x0800) != 0;
      EXT_PIN_13 = (pinState & 0x1000) != 0;
      EXT_PIN_14 = (pinState & 0x2000) != 0;
      EXT_PIN_15 = (pinState & 0x4000) != 0;
      EXT_PIN_16 = (pinState & 0x8000) != 0;

      //Print the pin states to Serial
      // Serial.print("EXT_PIN_1: "); Serial.print(EXT_PIN_1); Serial.print(", ");
      // Serial.print("EXT_PIN_2: "); Serial.print(EXT_PIN_2); Serial.print(", ");
      // Serial.print("EXT_PIN_3: "); Serial.print(EXT_PIN_3); Serial.print(", ");
      // Serial.print("EXT_PIN_4: "); Serial.print(EXT_PIN_4); Serial.print(", ");
      // Serial.print("EXT_PIN_5: "); Serial.print(EXT_PIN_5); Serial.print(", ");
      // Serial.print("EXT_PIN_6: "); Serial.print(EXT_PIN_6); Serial.print(", ");
      // Serial.print("EXT_PIN_7: "); Serial.print(EXT_PIN_7); Serial.print(", ");
      // Serial.print("EXT_PIN_8: "); Serial.print(EXT_PIN_8); Serial.print(", ");
      // Serial.print("EXT_PIN_9: "); Serial.print(EXT_PIN_9); Serial.print(", ");
      // Serial.print("EXT_PIN_10: "); Serial.print(EXT_PIN_10); Serial.print(", ");
      // Serial.print("EXT_PIN_11: "); Serial.print(EXT_PIN_11); Serial.print(", ");
      // Serial.print("EXT_PIN_12: "); Serial.print(EXT_PIN_12); Serial.print(", ");
      // Serial.print("EXT_PIN_13: "); Serial.print(EXT_PIN_13); Serial.print(", ");
      // Serial.print("EXT_PIN_14: "); Serial.print(EXT_PIN_14); Serial.print(", ");
      // Serial.print("EXT_PIN_15: "); Serial.print(EXT_PIN_15); Serial.print(", ");
      // Serial.print("EXT_PIN_16: "); Serial.print(EXT_PIN_16); Serial.println();
      
      right_indicator_on = EXT_PIN_2;
      left_indicator_on = EXT_PIN_3;
      brake_pedal_pressed = EXT_PIN_4;
      door_is_open = EXT_PIN_1;
      car_in_reverse = EXT_PIN_5;


      //Set the state of PIN
      // right_indicator_on = (digitalRead(RIGHT_INDICATOR) == HIGH);
      // left_indicator_on = (digitalRead(LEFT_INDICATOR) == HIGH);
      // brake_pedal_pressed = (digitalRead(BRAKE_PEDAL) == HIGH);
      // door_is_open = (digitalRead(DOOR_OPEN) == HIGH);

      const bool reverse_state_change = reverse_previous_state != car_in_reverse;
      reverse_previous_state = car_in_reverse;
      
      const bool brake_state_change = brake_pedal_previous_state != brake_pedal_pressed;
      brake_pedal_previous_state = brake_pedal_pressed;

      brake_effect_override = last_brake_press_time && (millis() - last_brake_press_time < brake_effect_duration);
      if(reverse_state_change || brake_state_change || brake_previously_set){
        if(brake_pedal_pressed || car_in_reverse){
          if(!brake_previously_set || reverse_state_change){
            strip.setTransition(0);
            preBrakeBrightness = strip.getBrightness();
            if(car_in_reverse){
              updateSegment(BRAKE_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0xFFC68C, 0xFFC68C, 255, 0);
            }else{
              updateSegment(BRAKE_SEGMENT, FX_MODE_STATIC, 0xFF0000, 0xFF0000, 0xFF0000, 255, 0);

            }
            Serial.println("++++++ BRAKE PRESSED ++++++");
          }  
          brake_previously_set = true;
          last_brake_press_time = millis();
        }else if(!brake_effect_override){
          //strip.setTransition(1000);
          //Don't reset fully if the door is open.
          if(!door_is_open && !door_effect_override){
            strip.setTransition(0);
            strip.setBrightness(preBrakeBrightness);
            restorePreviousState(BRAKE_SEGMENT, brake_segment_cache);
          }else{
            strip.setTransition(0);
            door_brake_segment_cache = brake_segment_cache;
            //Enable door mode.
            updateSegment(BRAKE_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);
          }
          Serial.println("------ BRAKE RELEASED ------");
          brake_previously_set = false;
          last_brake_press_time = 0;        
        }
      }else if(!(brake_pedal_pressed || car_in_reverse) && !brake_effect_override){
        //Stop caching state when door is open.
        if(!door_is_open && !door_effect_override){
          brake_segment_cache = strip.getSegment(BRAKE_SEGMENT);
        }
      }

      const bool left_state_change = left_indicator_previous_state != left_indicator_on;
      left_indicator_previous_state = left_indicator_on;

      left_effect_override = last_left_on_time && (millis() - last_left_on_time < left_effect_duration);
      if(left_state_change || left_previously_set){
        if(left_indicator_on){
          if(!left_previously_set){
            strip.setTransition(0);
            preLeftBrightness = strip.getBrightness();
            if(!door_is_open && !door_effect_override){
                left_segment_cache = strip.getSegment(LEFT_SEGMENT);
            }


            updateSegment(LEFT_SEGMENT, FX_MODE_RUNNING_COLOR, 0xFFAA00, 0x000000, 0x000000, 255, 0);
            Serial.println("++++++ LEFT CLICKER ON ++++++");
          }
          left_previously_set = true;
          last_left_on_time = millis();
          
        }else if(!left_effect_override){
          //Don't reset fully if the door is open.
          strip.setTransition(1000);
          if(!door_is_open && !door_effect_override){
            strip.setBrightness(preLeftBrightness);
            restorePreviousState(LEFT_SEGMENT, left_segment_cache);
          }else{
            //Pass the clicker cache to door cache
            door_left_segment_cache = left_segment_cache;
            //Enable door mode.
            updateSegment(LEFT_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);
          }
          Serial.println("------ LEFT CLICKER OFF ------");
          last_left_on_time = 0;
          left_previously_set = false;
        } 
      }else if(!left_indicator_on && !left_effect_override){
        //Stop caching state when door is open.
        if(!door_is_open && !door_effect_override){
          left_segment_cache = strip.getSegment(LEFT_SEGMENT);
        }
      }


      const bool door_state_change = door_previous_state != door_is_open;
      door_previous_state = door_is_open;

      door_effect_override = last_door_open_time && (millis() - last_door_open_time < door_effect_duration);
      if(door_state_change || door_previously_set){
        if(door_is_open){
          if(!door_previously_set){
            strip.setTransition(2000);
            preDoorBrightness = strip.getBrightness();
            door_front_segment_cache = strip.getSegment(FRONT_SEGMENT);

            if(!left_indicator_on && !left_effect_override){
              door_left_segment_cache = strip.getSegment(LEFT_SEGMENT);
              updateSegment(LEFT_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);

            }
            if(!right_indicator_on && !right_effect_override){
              door_right_segment_cache = strip.getSegment(RIGHT_SEGMENT);
              updateSegment(RIGHT_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);
            }
            if(!brake_pedal_pressed && !brake_effect_override){
              door_brake_segment_cache = strip.getSegment(BRAKE_SEGMENT);
              updateSegment(BRAKE_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);

            }
            updateSegment(FRONT_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);
            Serial.println("++++++ DOOR ACTIVATED ++++++");
          }
          door_previously_set = true;
          last_door_open_time = millis();
          
        }else if(!door_effect_override){
          strip.setTransition(2000);
          strip.setBrightness(preDoorBrightness);
          restorePreviousState(FRONT_SEGMENT, door_front_segment_cache);
          //Don't touch the clicker if they're on
          if(!left_indicator_on && !left_effect_override){
            restorePreviousState(LEFT_SEGMENT, door_left_segment_cache);
          }
          //Don't touch the clicker if they're on
          if(!right_indicator_on && !right_effect_override){
            restorePreviousState(RIGHT_SEGMENT, door_right_segment_cache);
          }
          //Don't touch the brake if it's being pressed
          if(!brake_pedal_pressed && !brake_effect_override){
            strip.setTransition(0);
            restorePreviousState(BRAKE_SEGMENT, door_brake_segment_cache);
          }
          Serial.println("------ DOOR DEACTIVATED ------");

          door_previously_set = false;
          last_door_open_time = 0;      
        }
      }else if(!door_is_open && !door_effect_override){
          
          


      }

      const bool right_state_change = right_indicator_previous_state != right_indicator_on;
      right_indicator_previous_state = right_indicator_on;
      right_effect_override = last_right_on_time && (millis() - last_right_on_time < right_effect_duration);
      if(right_state_change || right_previously_set){
        if(right_indicator_on){
          if(!right_previously_set){
            strip.setTransition(0);
            preRightBrightness = strip.getBrightness();
            updateSegment(RIGHT_SEGMENT, FX_MODE_RUNNING_COLOR, 0xFFAA00, 0x000000, 0x000000, 255, 0);
            Serial.println("++++++ RIGHT CLICKER ON ++++++");
          }
          
          right_previously_set = true;
          last_right_on_time = millis();

        }else if(!right_effect_override){
          //Don't reset fully if the door is open.
          strip.setTransition(1000);

          if(!door_is_open && !door_effect_override){
            strip.setBrightness(preRightBrightness);
            restorePreviousState(RIGHT_SEGMENT, right_segment_cache);
          }else{
            //Pass the clicker cache to door cache
            door_right_segment_cache = right_segment_cache;
            //Enable door mode.
            updateSegment(RIGHT_SEGMENT, FX_MODE_STATIC, 0xFFC68C, 0x000000, 0x000000, 255, 0);
          }
          Serial.println("------ RIGHT CLICKER OFF ------");
          
          right_previously_set = false;
          last_right_on_time = 0;      
        }
      }else if(!right_indicator_on && !right_effect_override){
        //Stop caching state when door is open.
        if(!door_is_open && !door_effect_override){
          right_segment_cache = strip.getSegment(RIGHT_SEGMENT);
        }
      }
    }
};