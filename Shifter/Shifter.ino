// Setup shifting pins
int latchPin = 2; // 2 attiny, 8 uno
int clockPin = 3; // 3 attiny, 12 uno
int dataPin = 0; // 0 attiny, 11 uno

// Setup limit switch
int limitPin = 4;

// Setup timing variables
long lakeFirstTime;
long lakeTime;

long riverFirstTime;
long riverTime;

long tempFirstTime;
long tempTime;

// Setup Array
int data[28];

void clearShift(){
  for(int i = 0; i < sizeof(data); i ++){
    data[i] = 0;
  }
  
  // Set up the lake
  for(int i = 10; i < 15; i ++){
    if(i%2 == 0){
      data[i] = 1;
    } else {
      data[i] = 0;
    }
  }
  
  pushArray();
}

void pushArray(){
  digitalWrite(latchPin, 0);
  for(int i = sizeof(data); i > 0; i --){
    digitalWrite(dataPin, data[i]);
    digitalWrite(clockPin, 1);
    digitalWrite(clockPin, 0);
  }
  digitalWrite(latchPin, 1);
}

// Shift the array down once
void shiftDown(int a, int b, int repl){
  for(int i = b; i > a; i --){
    data[i] = data[i-1];
  }
  data[a] = repl;
}

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // Setup limit switch with internal pullup
  pinMode(limitPin, INPUT_PULLUP);

  // Initialize the timer thread in milliseconds
  lakeFirstTime = millis();
  riverFirstTime = millis();
  
  // Setup the Array
  clearShift();
}

void loop() {
  riverTime = millis() - riverFirstTime;
  lakeTime = millis() - lakeFirstTime;
  if(riverTime > 100){
    // lower river 17-28
    shiftDown(16,27,data[9]);
    pushArray();
    
    // upper river, 1 - 10
    shiftDown(0,9,0);
    pushArray();
    

    riverFirstTime = millis();
  }
  
  // lake 11 - 16
  if(lakeTime > 1000){
    int dataTemp = data[15];
    for(int i = 15; i > 10; i --){
      data[i] = data[i-1];
    }
    data[10] = dataTemp;
    pushArray();
    lakeFirstTime = millis();
  }
  
  // Reset time to 0
  if(digitalRead(limitPin) == HIGH){
    tempFirstTime = millis();
  }
  
  // Start counting
  tempTime = millis() - tempFirstTime;
  if(digitalRead(limitPin) == LOW && tempTime <= 10){
    data[0] = 1;
    data[1] = 1;
  }
  
  // Reset Shifter
  if(tempTime > 1000){
    clearShift();
  }
}
