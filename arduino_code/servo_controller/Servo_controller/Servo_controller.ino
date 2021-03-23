#include <Servo.h>
Servo servoA;
Servo servoB;
Servo servoC;

#define pinA 5
#define pinB 6
#define pinC 7

int angle1 = 0;
int angle2 = 0;
int angle3 = 0;

int Delay = 18; //delay in ms

void setup()  {
  Serial.begin(38400);
  servoA.attach(5);

}

void loop() { // act only when you receive data
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT);
  if (Serial.available() > 0)
  {
    delay(20);
    int x = Serial.parseInt();
    Serial.end();
    Serial.begin(38400);
    
    if (x == 1) {
      delay(20);

      while (Serial.available() == 0)
      {
        Serial.println("Waiting");
        delay(2000);
      }
    }
    else
    {
      pinMode(7, OUTPUT);

      Serial.print("I received: ");
      Serial.println(x);
      Serial.print("I am going to: ");
      Serial.println(x);
      if (x > angle1)
      { //if the new “command” is large than previous angle
        //every time increase 1
        for (int i = angle1; i <= x; i++)
        { servoA.write(i);
          delay(Delay);
        };

      }
      else
      { //else decrease 1
        for (int i = angle1; i >= x; i--)
        { servoA.write(i);
          delay(Delay);
        };
      }
      //recorded the angle
      angle1 = x;
      Serial.println("I finished, what is your next angle?");
    }

    
  }

}
