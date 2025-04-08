#define Vout_PIN 3;

//define constants
const int mag_per_rev = 4;              //number of magnets per revolution
int sensor_state;             //state of the sensor (1 or 0);
const int wheel_radius = 0;         //wheel radius in ()

//Global Variables
volatile int pulse_current = 0;   //current number of pulse counted
volatile int pulse_last = 0;      //number of pulse counted at the last time interrupt
float last_velocity = 0;          //velocity of the wheel at the last time interrupt
float acceleration = 0;           //acceleration of the wheel at the last time interrupt

//Interrupt Service Routines
//function for external interrupt: counting the pulses everytime the sensor state changes
void pulse_count() {
  pulse_current += 1; 
}

//ISR for timer interrupt: updating the number of pulses and conducting calculations for wheels' (angular) velocity every 1s 
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0; //reset timer counter

  //Calculate the number of pulses within the last 1s
  int n = pulse_current - pulse_last;
  pulse_last = pulse_current;
  //Convert to RPM and Velocity using float for precision
  float RPS = n/ (float) mag_per_rev; //RPs = revolutions per second
  float RPM = RPS*60; //RPM = revolutions per minute
  float omega = 2 * PI * RPS; //Angular velocity in rad/s
  float linear_velocity = omega * wheel_radius; //Linear velocity in m/s
  acceleration = linear_velocity - last_velocity;
  last_velocity = linear_velocity;
  
  //Print results
  Serial.print("RPM: ");
  Serial.print(RPM);
  Serial.print(", Angular Velocity: ");
  Serial.print(omega);
  Serial.print(", Velocity: ");
  Serial.println(linear_velocity);
  
}

void setup() {
  Serial.begin(57600);
  pinMode(Vout_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(Vout_PIN), pulse_count, CHANGE);

  //Set up timer interrupt for 1 second
  TCCR1A = 0;           //Reset entire timer1 register A to 0
  TCCR1B = 0;           //Reset entire timer2 register B to 0
  TCNT1  = 0;           //Initialize the counter value
  TCCR1B |= B00000100;  //Set prescaler as 256
  TIMSK1 |= B00000010;  //Enable compare match mode on register A

  //To initiate timer pulse every 1s, register needs to count to the value: (1s)*(16MHz)/(256) = 62500
  OCR1A = 62500;        //Set the compare match value on register A where time interrupt occurs
  interrupts();         //Enable global interrupts
}

void loop() {
//   //Plot sensor state
//   sensor_state = digitalRead(Vout_PIN);
//   Serial.println(sensor_state);
}