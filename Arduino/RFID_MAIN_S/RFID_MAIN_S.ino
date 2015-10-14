void setup() 
{
  Serial.begin(9600);
  pinMode(9,INPUT);
  digitalWrite(9,HIGH);
  pinMode(10,INPUT);
  digitalWrite(10,HIGH);
  pinMode(11,INPUT);
  digitalWrite(11,HIGH);
  pinMode(12,OUTPUT);
  digitalWrite(12,HIGH);
}

void loop() 
{
  if(digitalRead(9)==HIGH&&digitalRead(10)==HIGH&&digitalRead(11)==HIGH)
  {
    digitalWrite(12,LOW);
  }
  else
  {
    
  }
}
