float refRes[3] = {1000,1000,1000}; 
float resistances[3][3];
float currents[3][3];
float currentSums[3][3];
float columnCurrents[3];
int nums[3][3];
bool match;

int speedPin=10;
int dir1=11;
int dir2=12;
int mSpeed=200;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(speedPin, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  
}

void allSourcesOff(){
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
}

void allSourcesOn(){
  analogWrite(5, 240);
  analogWrite(6, 240);
  analogWrite(9, 240);
}

void counterClockwise(){
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, LOW);
  analogWrite(speedPin, mSpeed);
}

void motorOff(){
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  analogWrite(speedPin, 0);
}

void computeCurrents(){
  columnCurrents[0]=0;
  columnCurrents[1]=0;
  columnCurrents[2]=0;
  for(int i=0;i<=2;i++){
    for(int j=0;j<=2;j++){
      int rowChar = i;
      int colChar = j;
      allSourcesOff();
      int val = 240; //for 33k HRS, this is 245 -- for 100k HRS, this is 240
      if(nums[rowChar][colChar]==1) val=113; //for 33k HRS, this is 123 -- for 100k HRS, this is 100
      for(int i=5;i<=6;i++){
        if(rowChar+5==i){
          analogWrite(i, val);
        }
        else{
          analogWrite(i, 0);
        }
      }
      if(rowChar==2){
        analogWrite(9, val);
        analogWrite(5, 0);
        analogWrite(6, 0);
      }
      delay(60);
      int analogValue = analogRead(colChar);
      float voltageRef = (5./1023.)*analogValue;
      float current = voltageRef/refRes[colChar];
      currents[i][j] = current*1000000; 
      //columnCurrents[j]+=current*1000000;
      currentSums[i][j]+=current*1000000;
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()==0){}
  String temp = Serial.readString();
  int counter = 0;
  Serial.println("You entered: ");
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      nums[i][j]=temp.substring(counter, counter+1).toInt();
      counter++;
      Serial.print(nums[i][j]); Serial.print("\t");
    }
    Serial.println();
    while(temp.charAt(counter)==' '){ counter++; }
  }
  //Average edition
  for(int i=0;i<4;i++){
    computeCurrents();
  }
  for(int i=0;i<3;i++){
    columnCurrents[i]=0;
  }
  //This code block with the for loop was part of the averaging edition
  for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
        currents[i][j]=currentSums[i][j]/4.;
        currentSums[i][j]=0;
        if(currents[i][j]<30 && currents[i][j]>24.44){
          currents[i][j]=24.44;
        }
        columnCurrents[j]+=currents[i][j];
        //Serial.println(columnCurrents[j]);
      }
  }
  if(columnCurrents[0]>=54 && columnCurrents[0]<=78 && columnCurrents[1]>=54 && columnCurrents[1]<=78 && columnCurrents[2]>=54 && columnCurrents[2]<=78){ //with 100k potentiometer, should be in ranges of 75-111
    Serial.println("There was a match!");
    match = true;
  }
  else{
    Serial.println("Did not match.");
    match = false;
  }
  /*
  Serial.println("Currents produced by each resistor: ");
    for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
        Serial.print(currents[i][j]); Serial.print("\t");
      }
      Serial.println();
    }
   */
  allSourcesOn();
  if(match){
    counterClockwise();
  }
  else{
    motorOff();
  }
  delay(100);
}
