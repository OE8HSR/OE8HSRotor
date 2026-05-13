/*
 * Firmware for OE8HSRotor Antenna Rotor Version 0.1
 * This Software uses Parts from the early SatNogs Arduino firmware. Check out their cool Project at www.satnogs.org
 * 
 * Use this Software and the OE8HSRotor Hardware at your own Risk.
 * Always keep in mind that Computer controlled Machines can be dangerous. 
 * Keep your fingers away from rotating parts even if they are small and made out of plastic.
 * 
 * Damages on:
 * Expensive Radio Hardware
 * Expensive Antenna Hardware
 * Expensive Rotor Hardware
 * Human Bodyparts
 * are not the Authors Fault.
 * 
 * Please keep in Mind that this code has been written by a Metallworker. If you think you can make it better and contribute please do so. =)
 */
 
 
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <AccelStepper.h>
#include <EEPROM.h>

/* Defining Output and Input Pins */
#define DIR_AZ 5 //PIN for Azimuth Direction X
#define STEP_AZ 2 //PIN for Azimuth Steps X
#define DIR_EL 6 //PIN for Elevation Direction Y
#define STEP_EL 3 //PIN for Elevation Steps Y
#define EN 8 //PIN for Enable or Disable Stepper Motors

#define LEDR 13 //PIN for the Red LED
#define LEDG 12 //PIN for the Green LED
#define ZEROSW 11 //PIN for the "Rotor Zeroing Switch"



#define SPR 200 //StepperMotor Stepps/Rev
#define RATIOEL 120 //Gear ratio Elevation: Stage1: 10->50 = 1:5 Stage2: 10->60 = 1:6 Total: 5 x 6 = 30 Elevation runs on quarter stepps so 30 x 4 = 120
#define RATIOAZ 40 //Gear ratio Azimuth: Stage1: 10->100 = 1:10 Azimuth runs on quarter stepps so 10 x 4 = 40


/*Defining Stepper Objects*/
AccelStepper AZstepper(1, STEP_AZ, DIR_AZ);
AccelStepper ELstepper(1, STEP_EL, DIR_EL);

int counter;
bool done;

void setup()
{  
  /*Change values that work for your*/
  AZstepper.setMaxSpeed(1000);
  AZstepper.setAcceleration(500);
  
  /*Change values that work for your*/
  ELstepper.setMaxSpeed(2000);
  ELstepper.setAcceleration(1000);
  
  /*Disable Steppers for zeroing by hand*/
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);
  
  /*Defining LED Outputs*/
  pinMode(LEDG, OUTPUT);
  digitalWrite(LEDG, LOW);
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, LOW);
  
  pinMode(ZEROSW, INPUT);
  digitalWrite(ZEROSW, HIGH);

  Serial.begin(19200);

  counter = 0;
  done = 0;
  

  /*Set Rotor Zero*/
  setZero();
}

void loop()
{ 
  static int AZstep = 0;
  static int ELstep = 0;
  

  cmd_proc(AZstep, ELstep);
  stepper_move(AZstep, ELstep);
  
}
  



/*SetZero*/
void setZero(void)
{
  
  
  while(digitalRead(ZEROSW) != LOW) 
  {
    digitalWrite(LEDR, HIGH);
    digitalWrite(EN, HIGH);
  }
  
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  digitalWrite(EN, LOW);
  
  
  ELstepper.setCurrentPosition(0); 
  AZstepper.setCurrentPosition(0); 
  
}
 
 

void cmd_proc(int &stepAz, int &stepEl)
{
  //Serial
  char buffer[256];
  char incomingByte;
  char data[256];
  
    
    

        if(Serial.available() > 0)
        {
          incomingByte = Serial.read(); // Read a character
          switch (incomingByte) 
          {
             case '\n':
             done = 1;

             break;
             
             case 'U':
             done = 1;

             break;
             
             default: 
             buffer[counter] = incomingByte; // Store it
             counter++; // Increment where to write next
             buffer[counter] = '\0'; // Null terminate the string
             break;
          }
         }


      
if(done)
{
  
         for(int i = 0; i < strlen(buffer); i++)
         {
           if (buffer[i] == 'A' && buffer[i + 1] == 'Z'  && buffer[i + 2] == ' ' && buffer[i + 3] == 'E' && buffer[i + 4] == 'L')
           {
             Serial.print("AZ");
             Serial.print(getAzAngle());
             Serial.print(" ");
             Serial.print("EL");
             Serial.println(getElAngle());

           }
           else if(buffer[i] == 'A' && buffer[i + 1] == 'Z' && isdigit(buffer[i + 2]))
           {
             strncpy(data, buffer+(i+2), 100);
             

             double angleAz = atof(data);

             data[0] = '\0';
 
             stepAz = deg2stepAZ(angleAz);   

             Serial.println("AZ");

           }
           else if(buffer[i] == 'E' && buffer[i + 1] == 'L' && isdigit(buffer[i + 2]))
           {
             strncpy(data, buffer+(i+2), 100);
             

             double angleEl = atof(data);
 
             data[0] = '\0';
 
              stepEl = (deg2stepEL(angleEl)) * -1;  
              Serial.println("EL"); 

           }
           else if (buffer[i] == 'S' && buffer[i + 1] == 'A' && buffer[i + 2] == ' ' && buffer[i + 3] == 'S' && buffer[i + 4] == 'E')
           {
             stepAz = AZstepper.currentPosition();
             stepEl = ELstepper.currentPosition();

           }
           else if (buffer[i] == 'R' && buffer[i + 1] == 'E' && buffer[i + 2] == 'S' && buffer[i + 3] == 'E' && buffer[i + 4] == 'T')
           {
             setZero();

             stepAz = 0;
             stepEl = 0;

           }      

  }
done = 0;
counter = 0;
buffer[0] = '\0';

}
}





/*Send pulses to stepper motor drivers*/
void stepper_move(int stepAz, int stepEl)
{
  AZstepper.moveTo(stepAz);
  ELstepper.moveTo(stepEl);
    
  AZstepper.run();
  ELstepper.run();
}

/*Convert degrees to steps*/
int deg2stepAZ(double deg)
{
  return(RATIOAZ*SPR*deg/360);
}

/*Convert steps to degrees*/
double step2degAZ(int Step)
{
  return(360*Step/(SPR*RATIOAZ));
}

int deg2stepEL(double deg)
{
  return(RATIOEL*SPR*deg/360);
}

/*Convert steps to degrees*/
double step2degEL(int Step)
{
  return(360*Step/(SPR*RATIOEL));
}

float getElAngle(void)
{
  return(step2degEL(ELstepper.currentPosition()));
}

float getAzAngle(void)
{
  return(step2degAZ(AZstepper.currentPosition()));
}

boolean isNumber(char *input)
{
  for (int i = 0; input[i] != '\0'; i++)
  {
    if (isalpha(input[i]))
      return false;
  }
   return true;
}

