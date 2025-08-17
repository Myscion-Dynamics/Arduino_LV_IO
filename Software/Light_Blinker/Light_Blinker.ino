// Code to control the Arduino LED light blinker
// This code is for the board in it's LED driver configuration
// This code toggles the IO outputs to blink the lights in a random pattern
// The random pattern is generated at the start of the program
// The code reads an analog input that is used to control how bright the LED's are
// The LED brightness is controlled by a software PWM driver
// A button can be used to enable and disable the LED lights
// If the code is not working make sure the potentiometer is set to max and try pressing the button

// These integers are used to asign the pin number to each pin name, this is done to make the code easier to read
const int io1 = 18;  // PC4
const int io2 = 17;  // PC3
const int io3 = 16;  // PC2
const int io4 = 15;  // PC1
const int io5 = 14;  // PC0
const int io6 = 2;   // PD2
const int io7 = 3;   // PD3
const int io8 = 4;   // PD4
const int io9 = 5;   // PD5
const int io10 = 6;  // PD6
const int io11 = 7;  // PD7
const int io12 = 8;  // PB0
const int io13 = 9;  // PB1
const int io14 = 10; // PB2
const int io15 = 11; // PB3
const int io16 = 12; // PB4
const int btn = 19;  // PC5
const int pot_pin = A7;
const int bat_sense_pin = A6;


//These variables can be changed to alter  performance
int threashold = 127; // Range 0-254, Used to compare to random numbers to determine if an LED is on, higher turs on more LED's
int cycleTime = 4000; //Milisceonds per cycle

// Do not change these variables
#define SQLENGTH 800 // How many steps in the sequence, if you go higher than 800 the arduino may run out of ram
uint16_t squence[SQLENGTH]; // Array to hold the seqence data
bool enabled = true; //Set to disabled if battery gets too low

void generateSequence(){// Generate a sequence of random numbers ahead of time
  for(int i = 0; i < SQLENGTH - 1; i++){ // This loops through each item in the array
    squence[i] = 0; // zero out the sequence if this is being re ran a 2nd time
    for(int ran_i = 0; ran_i < 16; ran_i++){ // This loops through the 16 bits on each item in the array
      int ran = random(0, 255);
      if (ran > threashold){ // The random number is between 0 and 255, the threashold can be used to bias the numbers on or off
        squence[i] = squence[i] | (1 << ran_i); // Shift 1 into the position that was just evaluated and store it in the sequence 
      }
    }
  }
}

// IO Range PC0-PC4, PB0-PB4, PD2-PD7
// Button = PC5
void writeSeqeunce(uint16_t data, float brightness, int wait){
  // This fuction takes in "data" and displays it at the speficied "brightness", the funcation uses "wait" to determine how long it should wait
  int ton = 1000 * brightness; // Calculate time on in micro seconds (us)
  int toff = 1000 * (1 - brightness); // Calcuate time off in us
  if (toff < 0) toff = 0; // Make sure time off never goes negative, due to how the potentiometer is sampled brightes can be higher than 100. This is a bit of a clumsy fix


  // For the sake of speed the PORT registers are used instead of the build in arduino digital write
  // Digital write is slow and this operation is much faster. For manual pin control there are three ports. PORTB, PORTC and PORTD
  // Each port has 3 registers, PORTB, PINB, DDRB, the registers control if the pin is on/off, input/output
  uint8_t PB = (data & 0b11111) | (PORTB & 0b11100000);
  uint8_t PC = ((data >> 5) & 0b11111) | (PORTC & 0b11100000);
  uint8_t PD = ((data >> 8) & 0b11111100) | (PORTD & 0b00000011);

  // This is the software brightness function, each cycle is 1 miliscond (ms), the delayMicroseconds command is used for higher accuracy
  for(int i = 0; i < wait; i++){
    PORTB = PB;
    PORTC = PC;
    PORTD = PD;
    delayMicroseconds(ton);

    PORTB &= 0b11100000;
    PORTC &= 0b11100000;
    PORTD &= 0b00000011;
    delayMicroseconds(toff);

    // Check if the button is pressed to turn the LED's off
    uint8_t btn_reg = PINC & 0b00100000;
    if(btn_reg == 0){
      enabled = false;
      delay(2000);
      break;
    }
  }

}

#define BAT_LOW 625 // Cut off at 10.5V, 2.06V on the divider
void checkBattery(){ // This function  checks if the battery is low
  int batt_reading = analogRead(bat_sense_pin);
  
  if(batt_reading < BAT_LOW){ // If the battery is low turn off all the lights
    uint8_t PB = (0b11111) | (PORTB & 0b11100000);
    uint8_t PC = (0b11111) | (PORTC & 0b11100000);
    uint8_t PD = (0b11111100) | (PORTD & 0b00000011);

    for(uint8_t i = 0; i < 5; i++){ // Flash the lights a few times
      PORTB = PB;
      PORTC = PC;
      PORTD = PD;
      delay(300);

      PORTB &= 0b11100000;
      PORTC &= 0b11100000;
      PORTD &= 0b00000011;
      delay(300);
    }

    while(true){ // Keep the lights off but flash them is the user preses the button
      uint8_t btn_reg = PINC & 0b00100000;
      if(btn_reg == 0){
        for(uint8_t i = 0; i < 5; i++){
          PORTB = PB;
          PORTC = PC;
          PORTD = PD;
          delay(300);

          PORTB &= 0b11100000;
          PORTC &= 0b11100000;
          PORTD &= 0b00000011;
          delay(300);
        }
      }
    }
  }
}

// The setup code runs once at the start
void setup() {
  // Configure the output mode of all the pins
  pinMode(io1, OUTPUT);
  pinMode(io2, OUTPUT);
  pinMode(io3, OUTPUT);
  pinMode(io4, OUTPUT);
  pinMode(io5, OUTPUT);
  pinMode(io6, OUTPUT);
  pinMode(io7, OUTPUT);
  pinMode(io8, OUTPUT);
  pinMode(io9, OUTPUT);
  pinMode(io10, OUTPUT);
  pinMode(io11, OUTPUT);
  pinMode(io12, OUTPUT);
  pinMode(io13, OUTPUT);
  pinMode(io14, OUTPUT);
  pinMode(io15, OUTPUT);
  pinMode(io16, OUTPUT);

  // Generate the sequence
  generateSequence();

  // Set the data direction registers. This may not be needed due to the pinMode commands 
  DDRB |= 0b00011111;
  DDRC |= 0b00011111;
  DDRD |= 0b11111100;
}

int sequencePos = 0;
int pot_reading = 0;
float brightness = 0;

// This loop runs continously
void loop() {
  sequencePos++;
  if(sequencePos > SQLENGTH - 1){ // Reset the seqence if it reaches the end
    sequencePos = 0;
  }

  // Read the analog pot and use it to set the brightness value
  pot_reading = analogRead(pot_pin);
  brightness = pot_reading/870.0;
  
  // If enabled run the next step of the seqence
  if(enabled){
    writeSeqeunce(squence[sequencePos], brightness, cycleTime);
  }else{
    for(int i = 0; i < 1000; i++){

      // Check if the button is pressed to turn the LED's back on
      uint8_t btn_reg = PINC & 0b00100000;
      if(btn_reg == 0){
        enabled = true;
        delay(5);
        while(btn_reg == 0){
          btn_reg = PINC & 0b00100000;
        }
        delay(5);

        break;
      }
      delayMicroseconds(1000);
    }
  }

  // Check the battery once per iteration
  checkBattery();

}
