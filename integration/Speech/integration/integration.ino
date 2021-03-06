/* integration.ino
** Final sketch for SIXT33N Speech version
** 
** EE16B Fall 2016
** Emily Naviasky & Nathaniel Mailoa
**
*/

// Define pins
// Note that if you change the encoder pins, you also need
// to change the setup function, the end of the loop 
// function and the ISR to enable appropriate pin interrupts
//     ask your GSI for help

#define LEFT_MOTOR P2_0
#define LEFT_ENCODER P2_5
#define RIGHT_MOTOR P1_5
#define RIGHT_ENCODER P1_2
#define MIC_ADC P6_0

// Define constants
#define TIMESTEP 200
#define HIGH_PWM 150
#define LOW_PWM 8
#define SIZE 2752
#define SIZE_AFTER_FILTER 172
#define INITIAL_PWM 130 // you might need to change this

// Operation modes
#define MODE_LISTEN 0
#define MODE_DRIVE 1
boolean TIMER_MODE = MODE_LISTEN;

// Timer period
#define TIMER_MS 0.35


/*--------------------------*/
/*       CODE BLOCK A1      */
/*--------------------------*/
// Enveloping and K-means constants
#define SNIPPET_SIZE 100 // YOUR CODE HERE 
#define PRELENGTH 5// YOUR CODE HERE 
#define THRESHOLD 0.5// YOUR CODE HERE 

#define KMEANS_THRESHOLD 0.0025 // YOUR CODE HERE 
#define LOUDNESS_THRESHOLD 700 // YOUR CODE HERE 

/*--------------------------*/
/*    END CODE BLOCK A1     */
/*--------------------------*/


/*--------------------------*/
/*       CODE BLOCK A3      */
/*--------------------------*/
//define arrays
float pca_vec1[SNIPPET_SIZE] = {0.0204715048746, 0.0106899791351, 0.00959835299711, 0.00523472097468, -0.00764873566005, -0.0346832168594, 0.0602004889621, 0.0623719008409, 0.103944013941, 0.128384782901, 0.132402537924, 0.0862806121469, 0.134921169388, 0.122687809624, 0.117858929059, 0.120459636332, 0.111795793858, 0.105502344433, 0.123431418634, 0.108046686824, 0.135775464966, 0.145241694931, 0.13604760477, 0.118984471709, 0.118126105591, 0.104389020216, 0.102202758619, 0.126587349973, 0.139242110273, 0.129841658129, 0.14720509392, 0.141223008451, 0.160856232088, 0.154336467007, 0.142343539168, 0.127604945042, 0.117304069907, 0.120334637724, 0.0966765928299, 0.0911556870442, 0.0753944133264, 0.0900139885149, 0.0622955234534, 0.0497508056132, 0.0341885459077, 0.00265879821523, -0.0374470125305, -0.0649407980624, -0.0800686736517, -0.0740645184111, -0.0785238701445, -0.077795122031, -0.0761103262313, -0.0733612302662, -0.0824049060021, -0.0988551842322, -0.10260114897, -0.0941976506714, -0.0947667728304, -0.102518326006, -0.0874764448273, -0.0642606337082, -0.0517326412474, -0.0468346048947, -0.0234114473326, -0.0172778228886, -0.0117522285838, -0.00625873594485, -0.00813328714968, -0.00433879330206, -0.00519453143206, -0.0172728938721, -0.00671917488729, -0.00217474654217, -0.0200433484688, -0.0325974778552, -0.0683753644663, -0.0928863016649, -0.0806035896049, -0.133591518585, -0.153595569126, -0.129789978557, -0.118780147478, -0.118254424969, -0.137260243481, -0.155302927295, -0.153980079565, -0.157969415208, -0.140359968197, -0.147923892204, -0.131945095457, -0.11296756381, -0.124120823349, -0.108126198182, -0.126564218584, -0.110962222819, -0.0997384825159, -0.0957565042303, -0.0727344553122, -0.0770079800809}; // YOUR CODE HERE
float pca_vec2[SNIPPET_SIZE] = {-0.0441721481537, -0.0419152350229, -0.0327134079229, -0.0414022762942, -0.00470682311858, -0.00916208794592, -0.0608549079583, -0.0422982372554, -0.0391978611156, -0.0465423157647, -0.0627051815745, -0.0614958681011, -0.0428860424471, -0.0472047436229, -0.0202525734741, -0.0260823027517, -0.0623139402173, -0.0663661059558, -0.113300229041, -0.132782590595, -0.170119348388, -0.188123143945, -0.173536317157, -0.163384343204, -0.187563407013, -0.174028323577, -0.16676190463, -0.144021432063, -0.0825792885524, -0.0294473681884, 0.0422852987042, 0.0975904979594, 0.0916019384554, 0.108398795907, 0.105101627822, 0.105843924301, 0.111327817337, 0.0968844245012, 0.100934331969, 0.0946203479447, 0.0985147303095, 0.0932680658441, 0.0989616539365, 0.117393012153, 0.119788518355, 0.100730822536, 0.046208530948, 0.0237145748004, 0.0125269438717, 0.0151143381195, 0.0144636453812, 0.0295322968507, 0.0443564078592, 0.0629190439358, 0.0727264756886, 0.0790442717874, 0.083026750641, 0.107726037173, 0.106545623526, 0.10564512791, 0.114496461306, 0.109869094329, 0.122903653249, 0.0930812565556, 0.133338097223, 0.138031422719, 0.142456581243, 0.136933183435, 0.143609170827, 0.145935572988, 0.137166936258, 0.122050134031, 0.124078037327, 0.0974510467947, 0.106508171367, 0.09112539323, 0.076955619835, 0.0591570484003, 0.011770815301, -0.00359959939119, -0.0360119732103, -0.0745267921613, -0.0699647863182, -0.0898894866943, -0.1015732249, -0.0997588373938, -0.1029505496, -0.124109391867, -0.108387292508, -0.121592679054, -0.116517561943, -0.101635178647, -0.0984509337741, -0.112253198721, -0.126670501022, -0.10692715488, -0.114126231259, -0.117610383664, -0.107955672035, -0.0812823888504}; // YOUR CODE HERE
float pca_vec3[SNIPPET_SIZE] = {-0.0100834001849, -0.00267728811961, -0.000135892397154, -0.00237724001523, -0.0208809987805, -0.0404105970281, 0.0431420823736, 0.0465721876011, 0.0186892729598, 0.00906734806144, -0.0292145623007, 0.0396135731768, -0.0173687505391, -0.00754399392818, 0.0222523230509, 0.0157895431539, -0.00441896441357, 0.0115699644808, -0.0221528542852, -0.0242905869437, -0.0693369221863, -0.0548497324734, -0.0231323618318, 0.00407903422165, -0.00691008790191, 0.00726593855012, 0.0250043993364, 0.00774568782531, 0.027006662496, 0.042221553388, 0.0201043863587, 0.00701644204571, -0.00694311067633, -0.011611841633, 0.00782212494321, 0.0251698567848, 0.00232438441098, 0.0308869159369, 0.0339467015099, 0.0561167895061, 0.0542367139419, 0.0598653284971, 0.0769614471408, 0.0536925207888, 0.119479587637, 0.097617930669, 0.178305719407, 0.225980738545, 0.22301271792, 0.224027186835, 0.220498559642, 0.22117165466, 0.209405229171, 0.222934579601, 0.155335825725, 0.134577177515, 0.12306764164, 0.150749355942, 0.117704942972, 0.0854877060674, 0.0397678865541, 0.0422695013073, 0.00825961989233, -0.0812114580375, -0.0824447309459, -0.145890748747, -0.128480547161, -0.148694403649, -0.183121621077, -0.14839804862, -0.172177169341, -0.161171715577, -0.156409007411, -0.152825334427, -0.194751843308, -0.174903326716, -0.188615814888, -0.179020345056, -0.161948936103, -0.0878751685713, -0.0995635302244, -0.123385217121, -0.0665499490427, -0.0952232260775, -0.0394871767258, -0.0405465640183, -0.0190495280516, 0.00224294945273, 0.00420983836599, -0.0167986582604, -0.0411729051675, -0.0482272508384, -0.0290941928377, 0.0150795333293, 0.0214616655038, 0.00944345608737, -0.0175149994176, -0.0140778454843, -0.00838016952827, -0.0389035689129}; // YOUR CODE HERE
float pca_vec4[SNIPPET_SIZE] = {-0.0682882780597, -0.0538108781088, 0.0149620786562, 0.05570731618, 0.0651760457997, -0.197577828391, -0.130623922184, -0.00521788482904, 0.0468994956237, 0.123315559235, 0.129024042612, 0.135829188042, 0.107273951726, 0.0943250095058, 0.141086042394, 0.11770342321, 0.111576286512, 0.0956446236739, 0.0362825557666, 0.0399862316695, -0.0016527167749, -0.0156825395053, -0.0296673530671, -0.00264657726433, -0.0198731559362, -0.000951016713442, -0.015557212441, -0.00746729294672, 0.0170871684933, 0.0127225947254, 0.012028777169, -0.0150834802755, -0.0906491340963, -0.0844722885024, -0.0873745151685, -0.115403590819, -0.114876965166, -0.143443502317, -0.135752071903, -0.119481587131, -0.131814104981, -0.126662050196, -0.181382570566, -0.171656429053, -0.183262724729, -0.194977760486, -0.118815283544, -0.0104896077322, -0.0165436704616, 0.0486037274037, 0.0319830132278, 0.0405282632301, 0.0440973798721, 0.0555111442467, 0.0611841541889, 0.0465298400019, 0.0338209437945, 0.0837111627346, 0.0728813330634, 0.104589239639, 0.133600608278, 0.170286875651, 0.211027883347, 0.208197183976, 0.169911640992, 0.130043734627, 0.107574107617, 0.0851114640717, 0.0609616976289, 0.0694271490908, 0.0608988240686, 0.0418037514339, 0.0528331962256, 0.0317066440888, 0.0431240392362, -0.0104329199227, -0.133398130139, -0.152602740492, -0.105106765759, -0.203754337457, -0.175193641555, -0.0780661310414, -0.095038046052, -0.125909653663, -0.120660980079, -0.0507994066865, -0.0887160152831, -0.0750625083405, -0.0689470092142, 0.00202848927706, -0.0567111746601, 0.00348463923196, 0.00770426492751, 0.00204284826408, 0.0390823661565, 0.113153239495, 0.108637565013, 0.120748016753, 0.0991681206029, 0.0789285112464}; // YOUR CODE HERE
float mean_vec[SNIPPET_SIZE] = {0.00431041197039, 0.00402934270977, 0.00411288889585, 0.00427355485926, 0.00619476278244, 0.013607147209, 0.0149080853075, 0.015222744457, 0.015080496995, 0.0153142309548, 0.0157117328957, 0.0163352514554, 0.0158936909767, 0.0157844289444, 0.0157627872106, 0.0155180038975, 0.0148180054321, 0.0139403969437, 0.0129683795252, 0.0125771832718, 0.0128150389908, 0.0123345033347, 0.0112286419986, 0.0111333694466, 0.010960573733, 0.0109694267771, 0.0114484510675, 0.0116541913805, 0.0117303209962, 0.0121275609006, 0.0127014644412, 0.0125326476276, 0.0128466100894, 0.0125749566657, 0.012635226451, 0.0118892789146, 0.0117066629614, 0.0121836778527, 0.0119799225863, 0.0117981892519, 0.0111382222908, 0.0113355182257, 0.0111968252016, 0.011071956162, 0.0115440661662, 0.0108067720369, 0.0123681837458, 0.012367767859, 0.0124689785944, 0.0116532667722, 0.0118505180846, 0.0118272369024, 0.0116694201721, 0.0121166619412, 0.0122156686666, 0.0119969950991, 0.0116180904319, 0.0120625710294, 0.0117484779034, 0.0114890407898, 0.0111762048792, 0.0103647115487, 0.0103021330189, 0.0100033513308, 0.00954676052258, 0.0090879327449, 0.00855569649336, 0.00794354908388, 0.00780211210513, 0.00760873059986, 0.00699278809835, 0.00659198701741, 0.00668207818091, 0.00627068597559, 0.00613554987621, 0.00572027369778, 0.00587276375666, 0.00614712183134, 0.00595469716149, 0.00636048995212, 0.0069818130922, 0.00673332839929, 0.006323420712, 0.00606472585213, 0.00669269286866, 0.00651194442116, 0.00668796707817, 0.00666919979616, 0.00643264607141, 0.00655474688443, 0.0064621769065, 0.00611821810559, 0.00591677899872, 0.00638301933867, 0.00652487997638, 0.00606143696395, 0.00590338948205, 0.00565217357311, 0.00513154360826, 0.00491379975948}; // YOUR CODE HERE
float centroid1[3] = {0.00493517, -0.00530989, -0.00493843    }; // YOUR CODE HERE
float centroid2[3] = {0.0103423,  -0.0023534,  -0.00163616}; // YOUR CODE HERE
float centroid3[3] = {-0.00581314, -0.00179676, -0.00657997 }; // YOUR CODE HERE
float centroid4[3] = {-0.00995251,  0.00876388,  0.01096291 }; // YOUR CODE HERE
float result[SNIPPET_SIZE] = {0};

/*--------------------------*/
/*    END CODE BLOCK A3     */
/*--------------------------*/


// Data array and index pointer
int i=0;
int re[SIZE]={0};
volatile int16_t re_pointer = 0;

//Control variables
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

long tempr, templ = 0;
float left_cur_pwm = (HIGH_PWM + LOW_PWM)/2;
float right_cur_pwm = (HIGH_PWM + LOW_PWM)/2;



/*--------------------------*/
/*       CODE BLOCK A2      */
/*--------------------------*/
// Target Velocity
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

/*--------------------------*/
/*    END CODE BLOCK A2     */
/*--------------------------*/



// drive_counter for how many times timestep since reset
int drive_count = 0;


/******************************************
*****          NORM FUNCTIONS         *****
*******************************************/
// Enveloping function with thresholding and normalizing,
// returns snippet of interest (containing speech)

bool envelope(int*, float*);

float l2_norm(float dim1, float dim2, float* centroid){
  return sqrt(pow(dim1-centroid[0],2) + pow(dim2-centroid[1],2));
}


float l2_norm3(float dim1, float dim2, float dim3, float* centroid){
  return sqrt(pow(dim1-centroid[0],2) + pow(dim2-centroid[1],2) + pow(dim3-centroid[2],2));
}

float inner_product(float* sample, float* component) {
  float sum = 0;
  for (int i = 0; i < SNIPPET_SIZE; i += 1) {
    sum += sample[i] * component[i];
  }
  return sum;
}

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

    // if enveloped data is above some preset value
    if(envelope(re, result)){

      /*--------------*/
      /* CODE BLOCK B */
      /*--------------*/

      for (int i = 0; i < SNIPPET_SIZE; i += 1) {
        result[i] -= mean_vec[i];
      }
      float proj[3] = {inner_product(result, pca_vec1), inner_product(result, pca_vec2), inner_product(result, pca_vec3)};

      // Classification - Hint: use the function l2_norm defined below
      // YOUR CODE HERE 
      int closest = 1;
      float closest_dist = l2_norm3(proj[0], proj[1], proj[2], centroid1);
      float d2 = l2_norm3(proj[0], proj[1], proj[2], centroid2);
      float d3 = l2_norm3(proj[0], proj[1], proj[2], centroid3);
      float d4 = l2_norm3(proj[0], proj[1], proj[2], centroid4);
      if (d2 < closest_dist) {
        closest = 2;
        closest_dist = d2;
      }
      if (d3 < closest_dist) {
        closest = 3;
        closest_dist = d3;
      }
      if (d4 < closest_dist) {
        closest = 4;
        closest_dist = d4;
      }

      if (closest == 1){ // shire
        v_star = 10;
        Serial.println("Straight");
        delta_offset = 0;
        drive_mode(255,255);
      } else if (closest == 4){ // obama
        v_star = 10;
        Serial.println("Left");
        delta_offset = 20;
        drive_mode(255,255);
      } else if (closest == 3){ // onetwothreefour
        Serial.println("Slow");
        v_star = 5;
        delta_offset = 0;
        drive_mode(255, 255);
        
      } else if (closest == 2){ // cheese
        v_star = 10;
        Serial.println("Right");
        delta_offset = -20;
        drive_mode(255,255);
      }
      /*---------------------*/
      /* END OF CODE BLOCK B */
      /*---------------------*/

    }

    delay(2000);
    re_pointer = 0;
  }
 
  else if(TIMER_MODE == MODE_DRIVE && do_loop){
    
    // Update speed data
    if (left_num_ticks > 0){
      left_current_speed = (float)left_num_ticks/(float)left_history;
    } else {
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
    
    //Serial.print(left_current_speed);
    //Serial.print('\t');
    //Serial.print(left_position);
    //Serial.print('\t');
    //Serial.print(right_current_speed);
    //Serial.print('\t');
    //Serial.println(right_position);
    
    
    

    /*--------------------------*/
    /*       CODE BLOCK C       */
    /*--------------------------*/
    
    // YOUR CODE HERE
    delta = left_position - right_position + delta_offset;
    left_cur_pwm = driveStraight_left(v_star, delta);
    right_cur_pwm = driveStraight_right(v_star, delta);
    /*--------------------------*/
    /*     END CODE BLOCK C     */
    /*--------------------------*/
    Serial.println(delta);
    // Saturation check
    if(left_cur_pwm > 255) left_cur_pwm = 255;
    if(left_cur_pwm < 0) left_cur_pwm = 0;
    if(right_cur_pwm > 255) right_cur_pwm = 255;
    if(right_cur_pwm < 0) right_cur_pwm = 0;

    // Send new PWM values
    analogWrite(LEFT_MOTOR, (int)left_cur_pwm);
    analogWrite(RIGHT_MOTOR, (int)right_cur_pwm); 
    
    // Counter for how many times loop is executed since entering DRIVE MODE
    drive_count++;

    // Stop motors and return to LISTEN MODE
    if (drive_count == 4*1000/TIMESTEP){
      re_pointer = 0;
      analogWrite(LEFT_MOTOR, 0);
      analogWrite(RIGHT_MOTOR, 0); 
      delay(3000); // 3 seconds buffer for mic cap settling
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

float driveStraight_left(float v_star, float delta){
  
  return (v_star + beta_left)/theta_left - kl/theta_left*delta;
}

float driveStraight_right(float v_star, float delta){

  return (v_star + beta_right)/theta_right + kr/theta_right*delta;
}

// Enveloping function with thresholding and normalizing,
// returns snippet of interest (containing speech)
bool envelope(int* data, float* data_out){
  int32_t avg = 0;
  float maximum = 0;
  int thres_index = 0;
  int32_t total = 0;
  int block;

  // Apply enveloping filter while finding maximum value
  for (block = 0; block < SIZE_AFTER_FILTER; block++){
    avg = 0;
    for (int i = 0; i < 16; i++) {
      avg += data[i+block*16];
    }
    avg = avg >> 4;
    data[block] = abs(data[block*16] - avg);
    for (int i = 1; i < 16; i++) {
      data[block] += abs(data[i+block*16] - avg);
    }
    if (data[block] > maximum) {
      maximum = data[block];
    }
  }
  
  // If not loud enough, return false
  if (maximum < LOUDNESS_THRESHOLD) return false;

  // Determine threshold
  float thres = THRESHOLD * maximum;

  // Figure out when interesting snippet starts and write to data_out
  block = PRELENGTH;
  while (data[block++] < thres);
  if (block > SIZE_AFTER_FILTER - SNIPPET_SIZE) 
    block = SIZE_AFTER_FILTER - SNIPPET_SIZE;
  for (int i = 0; i < SNIPPET_SIZE; i++){
    data_out[i] = data[block-PRELENGTH+i];
    total += data_out[i];
  }

  // Normalize data_out 
  for (int i = 0; i < SNIPPET_SIZE; i++)
    data_out[i] = data_out[i] / total;

  return true;
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
  delay(200);
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
  } else if (mode == MODE_DRIVE){
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
  } else if (TIMER_MODE == MODE_DRIVE){
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
