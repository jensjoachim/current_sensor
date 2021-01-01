
const int DBG_ENABLE = 0;

const int HYST_SEC_ON = 5;

// 0.096A = 24adc  -- 1adc = 0.004A = 4mA
const int TRIG_LEVEL_MA = 50;
const int TRIG_GAIN = 4;

const int SAMPLE_TICKS = 1785; // Matching 200ms

// PINS
const byte ALIVE_PIN = LED_BUILTIN;
const byte RELAY_PIN = 11; 
const byte SENSOR_PIN = A0;
const byte SW_PIN_PULL = 8;
const byte SW_PIN_LOW  = 9;

const byte LED_ON_LOW   = 6;
const byte LED_ON_HIGH  = 7;
const byte LED_OFF_LOW  = 2;
const byte LED_OFF_HIGH = 3;

int hyst_cnt;

// the setup function runs once when you press reset or power the board
void setup() {
  // Init pin for relay
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_ON_LOW, OUTPUT);
  pinMode(LED_ON_HIGH, OUTPUT);
  digitalWrite(LED_ON_HIGH, LOW);
  pinMode(LED_OFF_LOW, OUTPUT);
  pinMode(LED_OFF_HIGH, OUTPUT);
  digitalWrite(LED_OFF_HIGH, HIGH);
  // Init analog pins all outputs low
  digitalWrite(A0, LOW); 
  digitalWrite(A1, LOW); 
  digitalWrite(A2, LOW); 
  digitalWrite(A3, LOW); 
  digitalWrite(A4, LOW); 
  digitalWrite(A5, LOW); 
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  // Select Analog pin
  pinMode(SENSOR_PIN, INPUT);
  // Init switch input
  pinMode(SW_PIN_PULL, INPUT_PULLUP);
  pinMode(SW_PIN_LOW, OUTPUT);
  digitalWrite(SW_PIN_LOW, LOW);
  // Setup serial
  if (DBG_ENABLE == 1) {
    Serial.begin(9600);
  }
  // Set counter
  hyst_cnt = 0;
  
  // Wait some time
  delay(1000); 
}

unsigned long t_start;
unsigned long t_stop;
unsigned long t_diff;
int data_in;
int data_max;
int data[SAMPLE_TICKS];
long data_max_a;
long data_max_w;

// the loop function runs over and over again forever
void loop() {

    // Start Measurement
    t_start = millis();
    // Collect data
    data_max = 0;
    for (int i = 0; i < SAMPLE_TICKS; i++) {
      data_in = analogRead(SENSOR_PIN);
      data[i] = data_in;
      if (data_in > data_max) {
        data_max = data_in;
      }
    }
    // End measurement
    t_stop = millis();
    // Calc mili ampere
    data_max_a = data_max * TRIG_GAIN;
    data_max_w = (data_max_a * 23) / 100;
    // Enable relay
    if (digitalRead(SW_PIN_PULL)) {
      if (TRIG_LEVEL_MA < data_max_a) {
        // ON
        digitalWrite(RELAY_PIN, HIGH); 
        digitalWrite(LED_ON_HIGH, HIGH);
        digitalWrite(LED_OFF_HIGH, LOW);
        hyst_cnt = HYST_SEC_ON * 5;
      } else {
        if (hyst_cnt == 0) {
          // OFF
          digitalWrite(RELAY_PIN, LOW);
          digitalWrite(LED_ON_HIGH, LOW);
          digitalWrite(LED_OFF_HIGH, HIGH);
        } else {
          // ON
          digitalWrite(RELAY_PIN, HIGH);
          digitalWrite(LED_ON_HIGH, HIGH);
          //digitalWrite(LED_OFF_HIGH, LOW);
          digitalWrite(LED_OFF_HIGH, HIGH);
          hyst_cnt = hyst_cnt - 1;  
        }
      }
    } else {
      digitalWrite(RELAY_PIN, HIGH); 
      digitalWrite(LED_ON_HIGH, HIGH);
      digitalWrite(LED_OFF_HIGH, LOW);
    }
    // Print
    t_diff = t_stop - t_start;
    if (DBG_ENABLE == 1) {
      Serial.println("--");
      Serial.print("Time: ");
      Serial.println(t_diff);
      Serial.print("Max: ");
      Serial.println(data_max);
      Serial.print("Amp: ");
      Serial.print(data_max_a);
      Serial.println("mA");
      Serial.print("Watt: ");
      Serial.print(data_max_w);
      Serial.println("w");
    }
  
}
