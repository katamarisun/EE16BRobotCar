/* integration.ino
** Final sketch for SIXT33N Music version
** 
** EE16B Fall 2016
** Emily Naviasky & Nathaniel Mailoa
**
*/

#include "./fix_fft.c"

// Define pins
// Note that if you change the encoder pins you also need 
// to change the setup function, the end of the loop 
// function and the ISR to enable appropriate pin interrupts
//      ask your GSI for help

#define LEFT_MOTOR P2_0
#define LEFT_ENCODER P2_5
#define RIGHT_MOTOR P1_5
#define RIGHT_ENCODER P1_2
#define MIC_ADC P6_0

// Define constants
#define TIMESTEP 200
#define HIGH_PWM 150
#define LOW_PWM 80
#define SIZE 128
#define INITIAL_PWM 130 // you might need to change this

// Operation modes
#define MODE_LISTEN 0
#define MODE_DRIVE 1

boolean TIMER_MODE = MODE_LISTEN;
int i=0;

// Data array and index pointer
int8_t re[SIZE]={0};
int8_t im[SIZE]={0};
//float freq[SIZE] = {0};
volatile int re_pointer = 0;

// Timer period
#define TIMER_MS 0.35


/*---------------*/
/* CODE BLOCK A1 */
/*---------------*/
// YOUR CODE HERE:
#define FFT_LOWER_BOUND //YOUR CODE HERE
#define FFT_UPPER_BOUND //YOUR CODE HERE
#define NUM_DECISIONS //YOUR CODE HERE

float pca_vec[SIZE] = {...};
float pca_mean[SIZE] = {...};

int temp_decision = 0;
float freq[FFT_UPPER_BOUND - FFT_LOWER_BOUND] = {0};
/*----------------------*/
/* END OF CODE BLOCK A1 */
/*----------------------*/


// Loudness variables
int16_t loudness = 0;
float sum = 0.0;

// Control variables
boolean do_loop = 0;
float left_current_speed = 0; // in ms
long left_position = 0;
long left_last_time = 0;
int32_t left_history = 0;
int left_num_ticks = 0;

float right_current_speed = 0; // in ms
long right_position = 0;
long right_last_time = 0;
int32_t right_history = 0;
int right_num_ticks = 0;
float delta = 0;

byte genre_state = 0; // Genre of music heard

long tempr, templ = 0;
float left_cur_pwm = (HIGH_PWM + LOW_PWM)/2;
float right_cur_pwm = (HIGH_PWM + LOW_PWM)/2;


/*--------------------------*/
/*       CODE BLOCK A2      */
/*--------------------------*/
// Target Velocity
float v_star = //YOUR CODE HERE

// Estimated Parameters
float theta_left =  // YOUR CODE HERE
float theta_right = // YOUR CODE HERE
float beta_left =   // YOUR CODE HERE
float beta_right =  // YOUR CODE HERE

// Control Vector
float kl = // YOUR CODE HERE
float kr = // YOUR CODE HERE

//Hint: you may need to add a variable here...

/*------------------------------*/
/*      END CODE BLOCK A2       */
/*------------------------------*/



// drive_counter for how many times timestep since reset
int drive_count = 0;

void setup()
{  
  // Left wheel control and encoder
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT);

  // Right wheel control and encoder
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(RIGHT_ENCODER, INPUT);

  //microphone ADC
  pinMode(MIC_ADC, INPUT);
  pinMode(RED_LED, OUTPUT);

  // Turn on and set pin interrupts
  // If you change your encoder pins this block
  // needs to be modified
  P2IE |= BIT5; // P2.5 interrupt enabled
  P2IES = BIT5; // set to high edge
  P2IFG &= ~BIT5; // P2.5 IFG cleared
  P1IE |= BIT2; // P1.2 interrupt enabled
  P1IES = BIT2; // set to high edge
  P1IFG &= ~BIT2; // P1.2 IFG cleared

  Serial.begin(38400);

  // Stop wheels
  analogWrite(LEFT_MOTOR, 0);
  analogWrite(RIGHT_MOTOR, 0); 

  reset_blinker();

  // Set timer for timestep
  setTimer(MODE_LISTEN);
  __enable_interrupt();

  re_pointer = 0;
}



void loop()
{  
  if(TIMER_MODE == MODE_LISTEN && re_pointer == SIZE){  
    // Stop motor
    analogWrite(LEFT_MOTOR, 0);
    analogWrite(RIGHT_MOTOR, 0); 
    digitalWrite(RED_LED, LOW);

    // Loudness calculation (with filter); ranges from 2500 to 4500
    sum = 0;
    for (int i = 0; i < SIZE; i++){
      sum += abs(re[i]);
    }      
    loudness = loudness*0.5 + sum*0.5;
    Serial.print("loudness: ");
    Serial.println(loudness);

    // FFT
    // make sure imaginary is set to zero
    memset(im, 0, sizeof(im));
    // take a 128-point fft
    fix_fft(re,im,7,0);

    int identified_genre = 0;
    bool found_genre = 0;




    /*--------------*/
    /* CODE BLOCK B */
    /*--------------*/
    
    // Modify this section for your data processing/decision making
    // YOUR CODE HERE
    
    // Classification
    // YOUR CODE HERE 
    
    /*---------------------*/
    /* END OF CODE BLOCK B */
    /*---------------------*/


    /*--------------*/
    /* CODE BLOCK D */
    /*--------------*/

    if(found_genre){
      if(genre_state == identified_genre){
        //Drive straight in same direction
        //YOUR CODE HERE

        Serial.println("going straight");
        if(genre_state == 1){
          digitalWrite(GREEN_LED, HIGH);
          // Drive speed according to loudness - the louder the faster
          //YOUR CODE HERE
          drive_mode(0,0);
          digitalWrite(GREEN_LED, LOW);
        }
        else if (genre_state == 2){
          digitalWrite(RED_LED, HIGH);
          // Drive speed according to loudness - the louder the slower
          // YOUR CODE HERE
          drive_mode(0,0);
          digitalWrite(RED_LED, LOW);
        }
      } else if (identified_genre == 1) { 
  
        // YOUR CODE HERE: Make SIXT33N turn left
        
        // END OF YOUR CODE HERE
  
        genre_state = identified_genre;
        Serial.println("turning right");
      } else if (identified_genre == 2) {
  
        // YOUR CODE HERE: Make SIXT33N turn right
        
        // END OF YOUR CODE HERE
  
        genre_state = identified_genre;
        Serial.println("turning left");
      }
    }

    /*---------------------*/
    /* END OF CODE BLOCK D */
    /*---------------------*/




    re_pointer = 0;
  }

  else if(TIMER_MODE == MODE_DRIVE && do_loop){

    // Update speed data
    if (left_num_ticks > 0){
      left_current_speed = (float)left_num_ticks/(float)left_history;
    } 
    else {
      left_current_speed = 1.0/200;
    }
    left_history = 0;
    left_num_ticks = 0;

    if (right_num_ticks > 0){
      right_current_speed = (float)right_num_ticks/(float)right_history;
    } 
    else {
      right_current_speed = 1.0/200;
    }
    right_history = 0;
    right_num_ticks = 0;


    /*--------------------------*/
    /*       CODE BLOCK C       */
    /*--------------------------*/
    
    // YOUR CODE HERE
    delta = (...);
    left_cur_pwm = (...);
    right_cur_pwm = (...);

    /*--------------------------*/
    /*     END CODE BLOCK C     */
    /*--------------------------*/
    
    // Saturation Check
    if(left_cur_pwm > 255) left_cur_pwm = 255;
    if(left_cur_pwm < 0) left_cur_pwm = 0;
    if(right_cur_pwm > 255) right_cur_pwm = 255;
    if(right_cur_pwm < 0) right_cur_pwm = 0;  
   
    // Send new PWM values
    analogWrite(LEFT_MOTOR, (int)left_cur_pwm);
    analogWrite(RIGHT_MOTOR, (int)right_cur_pwm); 

    // Counter for how many times loop is executed since entering DRIVE MODE
    drive_count++;
    
    // ** Debug Print Statements **
    //     comment out if unneeded
    //Serial.print(left_current_speed);
    //Serial.print('\t');
    //Serial.print(left_position);
    //Serial.print('\t');
    //Serial.print(right_current_speed);
    //Serial.print('\t');
    //Serial.println(right_position);
    //Serial.print('\t');
    //Serial.println(drive_count);

    // Stop motors and return to LISTEN MODE 
    if (drive_count == 3*1000/TIMESTEP){
      analogWrite(LEFT_MOTOR, 0);
      analogWrite(RIGHT_MOTOR, 0); 
      delay(3000); // 3 seconds buffer for mic cap settling
      re_pointer = 0;
      TIMER_MODE = MODE_LISTEN;
      setTimer(MODE_LISTEN);
    }

    do_loop = 0;    
  }

  // Encoder reading for wheel not moving
  // Needs to be modified if encoder pins changed
  long temp1 = millis();
  if (temp1 - right_last_time > 1000) {
    P1IFG |= BIT2;
  }
  if (temp1 - left_last_time > 1000) {
    P2IFG |= BIT5;
  }
}


/******************************************
*****          HELPER FUNCTIONS       *****
*******************************************/
// Change to DRIVE MODE
void drive_mode(float left_push, float right_push){
  // Reset variables
  drive_count = 0;
  left_position = 0;
  right_position = 0;
  TIMER_MODE = MODE_DRIVE;

  // Enter DRIVE MODE
  setTimer(MODE_DRIVE);

  // Send starting pulse to start motor movement
  analogWrite(LEFT_MOTOR, left_push);
  analogWrite(RIGHT_MOTOR, right_push); 

  // Trigger encoder readings
  right_history = 0;
  right_num_ticks = 0;
  left_history = 0;
  left_num_ticks = 0;
  P1IFG |= BIT2;
  P2IFG |= BIT5;
  delay(200);    // Apply push to the motors for 200 ms
}


// Port 2 ISR for left encoder
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  if (P2IFG & BIT5){
    templ = millis();
    left_history += templ - left_last_time;
    left_last_time = templ;
    left_position += 1; //cm
    left_num_ticks += 1;
    P2IFG &= ~BIT5; // P2.5 IFG cleared
  }
}

// Port 1 ISR for right encoder
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  if (P1IFG & BIT2){
    tempr = millis();
    right_history += tempr - right_last_time;
    right_last_time = tempr;
    right_position += 1; //cm
    right_num_ticks += 1;
    P1IFG &= ~BIT2; 
  }
}

// Set timer for timestep; use A2 since A0 & A1 are used by PWM
void setTimer(boolean mode){
  if (mode == MODE_LISTEN){
    // Set the timer based on 25MHz clock
    TA2CCR0 = (unsigned int)(25000*TIMER_MS);  
    TA2CCTL0 = CCIE;             
    __bis_SR_register(GIE);
    TA2CTL = TASSEL_2 + MC_1 + TACLR + ID_0;
  } 
  else if (mode == MODE_DRIVE){
    // Set the timer based on 32kHz clock
    TA2CCR0 = (unsigned int)(32.768*TIMESTEP);
    TA2CCTL0 = CCIE;             // enable interrupts for Timer A
    __bis_SR_register(GIE);
    TA2CTL = TASSEL_1 + MC_1 + TACLR + ID_0;
  }
  TIMER_MODE = mode;
}

// ISR for timestep
#pragma vector=TIMER2_A0_VECTOR    // Timer A ISR
__interrupt void Timer2_A0_ISR( void )
{
  if (TIMER_MODE == MODE_LISTEN){
    if(re_pointer < SIZE){
      digitalWrite(RED_LED, HIGH);
      re[re_pointer] = (analogRead(MIC_ADC) >> 4) - 128;
      re_pointer += 1;
    }
  } 
  else if (TIMER_MODE == MODE_DRIVE){
    do_loop = 1;
  }
}

void reset_blinker(){
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(GREEN_LED, LOW);
}
