/* turning.ino
** Implementing turns in SIXT33N
** 
** EE16B Fall 2016
** Emily Naviasky & Nathaniel Mailoa
**
*/

// Define pins
#define LEFT_MOTOR P2_0
#define LEFT_ENCODER P2_5
#define RIGHT_MOTOR P1_5
#define RIGHT_ENCODER P1_2

//define constants
#define SIZE 150
#define TIMESTEP 200
#define HIGH_PWM 150
#define LOW_PWM 8
#define INITIAL_PWM 130 // you might need to change this

// Operation modes
#define MODE_LISTEN 0
#define MODE_DRIVE 1

boolean TIMER_MODE = MODE_LISTEN;
int i=0;

volatile int re_pointer = 0;

// Timer period
#define TIMER_MS 0.35

//Control variables
boolean do_loop = 0;
float left_target_speed = 0.06;
float left_current_speed = 0; // in ms
long left_position = 0;
long left_last_time = 0;
int32_t left_history = 0;
int left_num_ticks = 0;
int left_pointer = 0;

float right_target_speed = 0.06;
float right_current_speed = 0; // in ms
long right_position = 0;
long right_last_time = 0;
int32_t right_history = 0;
int right_num_ticks = 0;
int right_pointer = 0;

long tempr, templ = 0;
float left_cur_pwm = (HIGH_PWM + LOW_PWM)/2;
float right_cur_pwm = (HIGH_PWM + LOW_PWM)/2;


/******************************************
*****   Code from closed_loop.ino     *****
*******************************************/

/////////// Set target velocity //////////
float v_star = 10;

// Estimated Parameters
float theta_left = 0.0873757855057;
float theta_right = 0.0870450320672;
float beta_left = -1.65578298261;
float beta_right = -1.54992516939;

// Control Vector
float kl =  0.75;//-.75;
float kr =  .20;//.45;

//Hint: you may need to add a variable here...
int delta = 0;
int delta_offset = 0;

/******************************************
*****   End code from closed_loop.ino *****
******************************************/

// Counter for how many times timestep since reset
int drive_count = 0;
int dir = 0;

void setup()
{  
  // Left wheel control and encoder
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT);
  
  // Right wheel control and encoder
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(RIGHT_ENCODER, INPUT);

  pinMode(RED_LED, OUTPUT);

  // Start motor
  analogWrite(LEFT_MOTOR, (int)left_cur_pwm);
  analogWrite(RIGHT_MOTOR, (int)right_cur_pwm); 

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
  dir = 0;
  
  // Set tier for timestep
  setTimer(MODE_LISTEN);
  __enable_interrupt();
}



void loop()
{  
  if(TIMER_MODE == MODE_LISTEN){

    /*--------------*/
    /* CODE BLOCK D */
    /*--------------*/
  
    if (dir == 0){ // first 3 seconds - go straight
      dir = 1;
      //YOUR CODE HERE
      Serial.println("Straight");
      delta_offset = 0;
      wheel_drive(255,255);
    } else if (dir == 1){ // second 3 seconds - turn left
      dir = 2;
      // YOUR CODE HERE
      Serial.println("Left");
      delta_offset = 20;
      wheel_drive(255,255);
    } else if (dir == 2){ // third 3 seconds - go straight
      dir = 3;
      //YOUR CODE HERE
      Serial.println("Straight");
      delta_offset = 0;
      wheel_drive(255, 255);
    } else if (dir == 3){ // fourth 3 seconds - turn right
      dir = 0;
      // YOUR CODE HERE
      Serial.println("Right");
      delta_offset = -20;
      wheel_drive(255,255);
    }


    
    drive_count = 0;
    TIMER_MODE = MODE_DRIVE;
    // Enter DRIVE MODE
    setTimer(MODE_DRIVE);
    // Trigger encoder readings
    right_history = 0;
    right_num_ticks = 0;
    left_history = 0;
    left_num_ticks = 0;
    P1IFG |= BIT2;
    P2IFG |= BIT5;
    delay(200);
    
    /*---------------------*/
    /* END OF CODE BLOCK D */
    /*---------------------*/

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
    } else {
      right_current_speed = 1.0/200;
    }
    right_history = 0;
    right_num_ticks = 0;

    // Debug data - comment out if unneeded
    //Serial.print(left_current_speed);
    //Serial.print('\t');
    //Serial.print(left_position);
    //Serial.print('\t');
    //Serial.print(right_current_speed);
    //Serial.print('\t');
    //Serial.println(right_position);

    delta = left_position - right_position + delta_offset;

/******************************************
*****   Code from closed_loop.ino     *****
*******************************************/

    // Input into open loop (u: change PWM)
    // YOUR CODE HERE
    input_left(driveStraight_left(v_star, delta));
    input_right(driveStraight_right(v_star, delta));

    Serial.println(left_cur_pwm);
    Serial.println(right_cur_pwm);
    Serial.println(delta);
    Serial.println("------------------------");



/******************************************
***** End code from closed_loop.ino   *****
*******************************************/


    // Send new PWM values
    analogWrite(LEFT_MOTOR, (int)left_cur_pwm);
    analogWrite(RIGHT_MOTOR, (int)right_cur_pwm); 
     
    // Counter for how many times loop is executed since entering DRIVE MODE
    drive_count++;
//    Serial.println(drive_count);

    if (drive_count == 3*1000/TIMESTEP){
//      Serial.println("ndfkvn");
      // Completely stop and go back to listen MODE after 3 seconds
      analogWrite(LEFT_MOTOR, 0);
      analogWrite(RIGHT_MOTOR, 0); 
      delay(1000); // 1 seconds buffer 
      re_pointer = 0;
      TIMER_MODE = MODE_LISTEN;
      setTimer(MODE_LISTEN);
    }
    
    do_loop = 0;    
  }
  
  // Encoder reading for wheel not moving
  long temp1 = millis();
  if (temp1 - right_last_time > 1000) {
    P1IFG |= BIT2;
  }
  if (temp1 - left_last_time > 1000) {
    P2IFG |= BIT5;
  }
}

/******************************************
*****       Code from closed_loop.ino *****
*******************************************/
//YOUR CODE HERE
float driveStraight_left(float v_star, float delta){
  
  return (v_star + beta_left)/theta_left - kl/theta_left*delta;
}

float driveStraight_right(float v_star, float delta){

  return (v_star + beta_right)/theta_right + kr/theta_right*delta;
}

/******************************************
*****   End code from closed_loop.ino *****
*******************************************/


// Helper function to set new target velocities and change to DRIVE MODE
void wheel_drive(float left_input, float right_input){
  // Reset variables
  drive_count = 0;
  left_position = 0;
  right_position = 0;
  TIMER_MODE = MODE_DRIVE;
//  left_target_speed = left_vel;
//  right_target_speed = right_vel;

  // Enter DRIVE MODE
  setTimer(MODE_DRIVE);

  // Send starting pulse to start motor movement
//  left_cur_pwm = INITIAL_PWM;
//  right_cur_pwm = INITIAL_PWM;
//  if (left_vel > 0) 
  analogWrite(LEFT_MOTOR, left_input);
//  if (right_vel > 0) 
  analogWrite(RIGHT_MOTOR, right_input); 

  // Trigger encoder readings
  right_history = 0;
  right_num_ticks = 0;
  left_history = 0;
  left_num_ticks = 0;
  P1IFG |= BIT2;
  P2IFG |= BIT5;
  delay(200);
}

// New PWM signal for left controller
void input_left(float in){
  left_cur_pwm = in;
  // Protect for saturation
  if(left_cur_pwm > 255) left_cur_pwm = 255;
  if(left_cur_pwm < 0) left_cur_pwm = 0;
}

// New PWM signal for right controller
void input_right(float in){
  right_cur_pwm = in;
  // Protect for saturation
  if(right_cur_pwm > 255) right_cur_pwm = 255;
  if(right_cur_pwm < 0) right_cur_pwm = 0;
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
