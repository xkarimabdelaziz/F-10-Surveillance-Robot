#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(8,9);

const byte address[6] = "00001";

Servo myServo;

#define trigPin 2
#define echoPin A0
#define buzzer 10
#define flamePin A1
#define rainPin A2
#define pirPin A3
#define motionLed A4

int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;

long duration;
int distance;

struct DataPackage
{
char move;
bool scan;
};

DataPackage data;

unsigned long lastServoMove = 0;
int servoState = 0;

unsigned long lastBuzzTime = 0;
bool buzzerState = false;

void setup()
{

pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);

pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);

pinMode(buzzer,OUTPUT);

pinMode(flamePin,INPUT);
pinMode(rainPin,INPUT);
pinMode(pirPin,INPUT);

pinMode(motionLed,OUTPUT);

Serial.begin(9600);

myServo.attach(3);

radio.begin();
radio.setPALevel(RF24_PA_LOW);
radio.setDataRate(RF24_250KBPS);

radio.openReadingPipe(0,address);
radio.startListening();

}

void loop()
{

// حركة العربية

if(radio.available())
{
radio.read(&data,sizeof(data));
controlMotors(data.move);
}


// حركة السيرفو

if(millis() - lastServoMove > 1500)
{

if(servoState == 0)
{
myServo.write(0);
servoState = 1;
}

else if(servoState == 1)
{
myServo.write(90);
servoState = 2;
}

else if(servoState == 2)
{
myServo.write(180);
servoState = 3;
}

else
{
myServo.write(90);
servoState = 0;
}

lastServoMove = millis();

}


// PIR Motion LED

if(digitalRead(pirPin) == HIGH)
{
digitalWrite(motionLed,HIGH);
}
else
{
digitalWrite(motionLed,LOW);
}


// Flame detection

if(digitalRead(flamePin) == HIGH)
{

Serial.println("FLAME DETECTED");

if(millis() - lastBuzzTime > 80)
{
buzzerState = !buzzerState;
digitalWrite(buzzer,buzzerState);
lastBuzzTime = millis();
}

return;

}


// Rain detection

if(digitalRead(rainPin) == LOW)
{

Serial.println("RAIN DETECTED");

if(millis() - lastBuzzTime > 200)
{
buzzerState = !buzzerState;
digitalWrite(buzzer,buzzerState);
lastBuzzTime = millis();
}

return;

}


// Ultrasonic detection

digitalWrite(trigPin,LOW);
delayMicroseconds(2);

digitalWrite(trigPin,HIGH);
delayMicroseconds(10);

digitalWrite(trigPin,LOW);

duration = pulseIn(echoPin,HIGH,30000);

if(duration == 0)
distance = 999;
else
distance = duration * 0.034 / 2;

Serial.print("Distance: ");
Serial.println(distance);


// Obstacle alert

if(distance <= 5)
{

if(millis() - lastBuzzTime > 120)
{
buzzerState = !buzzerState;
digitalWrite(buzzer,buzzerState);
lastBuzzTime = millis();
}

}

else if(distance <= 10)
{

if(millis() - lastBuzzTime > 400)
{
buzzerState = !buzzerState;
digitalWrite(buzzer,buzzerState);
lastBuzzTime = millis();
}

}

else
{

digitalWrite(buzzer,LOW);
buzzerState = false;

}

}


void controlMotors(char command)
{

if(command=='F') forward();
else if(command=='B') backward();
else if(command=='L') left();
else if(command=='R') right();
else stopCar();

}


void forward()
{

digitalWrite(IN1,HIGH);
digitalWrite(IN2,LOW);

digitalWrite(IN3,HIGH);
digitalWrite(IN4,LOW);

}


void backward()
{

digitalWrite(IN1,LOW);
digitalWrite(IN2,HIGH);

digitalWrite(IN3,LOW);
digitalWrite(IN4,HIGH);

}


void left()
{

digitalWrite(IN1,LOW);
digitalWrite(IN2,HIGH);

digitalWrite(IN3,HIGH);
digitalWrite(IN4,LOW);

}


void right()
{

digitalWrite(IN1,HIGH);
digitalWrite(IN2,LOW);

digitalWrite(IN3,LOW);
digitalWrite(IN4,HIGH);

}


void stopCar()
{

digitalWrite(IN1,LOW);
digitalWrite(IN2,LOW);

digitalWrite(IN3,LOW);
digitalWrite(IN4,LOW);

}