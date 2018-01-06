/* open_loop.ino
**
** EE16B Fall 2016
** John Maidens, Emily Naviasky & Nathaniel Mailoa
**
*/

// Define pins
#define LEFT_MOTOR P2_0
#define LEFT_ENCODER P2_5
#define RIGHT_MOTOR P1_5
#define RIGHT_ENCODER P1_2

// Define constants
#define SIZE 5*20 // set horizon of 20 seconds
#define TIMESTEP 200
#define PWM_DELTA 6
int i=0;
int count = 0;
boolean do_loop = 0; // timer signal to increment timestep
long tempr, templ = 0;
boolean up = 1;

// Recording variables
int left_position = 0;
long left_last_time = 0;
int32_t left_history = 0;
int left_num_ticks = 0;
int left_pointer = 0;
int right_position = 0;
long right_last_time = 0;
int32_t right_history = 0;
int right_num_ticks = 0;
int right_pointer = 0;

// pwm value variables 
float left_cur_pwm  = 0.0;
float right_cur_pwm = 0.0;

/*--------------------------*/
/*       CODE BLOCK 1       */
/*--------------------------*/

// Target Velocity
float v_star = 

// Estimated Parameters
float theta_left =
float theta_right =
float beta_left =
float beta_right =

/*------------------------------*/
/*      END CODE BLOCK 1        */
/*------------------------------*/

void setup()
{  
  // Left wheel control and encoder
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT);
  
  // Right wheel control and encoder
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(RIGHT_ENCODER, INPUT);

  pinMode(RED_LED, OUTPUT);

  // Turn off motors and wait 5 seconds
  analogWrite(LEFT_MOTOR, 0);
  analogWrite(RIGHT_MOTOR, 0); 
  delay(5000);

  // Turn on and set pin interrupts
  P2IE |= BIT5; // P2.5 interrupt enabled
  P2IES = BIT5; // set to high edge
  P2IFG &= ~BIT5; // P2.5 IFG cleared
  P1IE |= BIT2; // P1.2 interrupt enabled
  P1IES = BIT2; // set to high edge
  P1IFG &= ~BIT2; // P1.2 IFG cleared

  Serial.begin(38400);
  
  reset_blinker();
  
  // Set timer for timestep
  setTimer();
  
  __enable_interrupt();
  
  // jolt the motors to get them started 
  analogWrite(LEFT_MOTOR,  255);
  analogWrite(RIGHT_MOTOR, 255); 
}



void loop()
{  
  if(do_loop){
    // If not done collecting data
    if(count < SIZE){
      count++;
      
      /*--------------------------*/
      /*       CODE BLOCK 2       */
      /*--------------------------*/
      
      // drive straight using open loop control 
      // Compute the pwm input required for each wheel based on v_star
      left_cur_pwm =
      right_cur_pwm =
      
      /*------------------------------*/
      /*      END CODE BLOCK 2       */
      /*------------------------------*/
      
      // Saturation Check
      if(left_cur_pwm > 255) left_cur_pwm = 255;
      if(left_cur_pwm < 0) left_cur_pwm = 0;
      if(right_cur_pwm > 255) right_cur_pwm = 255;
      if(right_cur_pwm < 0) right_cur_pwm = 0; 
      
      analogWrite(LEFT_MOTOR,  (int) left_cur_pwm);
      analogWrite(RIGHT_MOTOR, (int) left_cur_pwm); 

      // Print statements to help with debugging
      Serial.print("left_cur_pwm = ");
      Serial.println(left_cur_pwm);
      Serial.print("right_cur_pwm = ");
      Serial.println(right_cur_pwm);
    } 
    
    else { // When count has reached SIZE  
      // Turn off motors
      analogWrite(LEFT_MOTOR, 0);
      analogWrite(RIGHT_MOTOR, 0); 
    }
    do_loop = 0;
  }
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
void setTimer(){
  TA2CCR0 = (unsigned int)(32.768*TIMESTEP);       // set the timer based on 32kHz clock
  TA2CCTL0 = CCIE;             // enable interrupts for Timer A
  __bis_SR_register(GIE);
  TA2CTL = TASSEL_1 + MC_1 + TACLR + ID_0;
}

// ISR for timestep
#pragma vector=TIMER2_A0_VECTOR    // Timer A ISR
__interrupt void Timer2_A0_ISR( void )
{
  do_loop = 1;
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
