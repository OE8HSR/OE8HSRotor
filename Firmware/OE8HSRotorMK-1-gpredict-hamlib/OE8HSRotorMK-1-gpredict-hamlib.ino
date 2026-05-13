/*
 * OE8HSRotorMK-1 — Test-Firmware für gpredict / rotctld (Hamlib EasyComm II)
 *
 * rotctld (Seriell → TCP für gpredict) — Baudrate wie Serial.begin() unten (19200):
 *
 *   rotctld -m 202 -s 19200 -r /dev/ttyUSB0 -t 2000 -T 127.0.0.1
 *
 *   -m 202      EasyComm II
 *   -s 19200    wie Serial.begin(19200) in dieser Firmware
 *   -r …       serielles Gerät ggf. anpassen (ttyUSB0, ttyACM0, …)
 *   -t 2000    TCP-Port für gpredict
 *   -T 127.0.0.1  nur localhost
 *
 * Ausgangsbasis: OE8HSRotorMK-1.ino (unverändert im Repo belassen).
 *
 * Hintergrund:
 * Hamlib easycomm_rot_get_position() sendet nacheinander "AZ\n" und "EL\n"
 * und erwartet je eine Zeile "AZ<winkel>" bzw. "EL<winkel>" (sscanf "AZ%f" / "EL%f").
 * Die Original-Firmware antwortet nur auf die zusammengefasste Abfrage "AZ EL",
 * daher schlagen Positionsabfragen fehl — gpredict kann beim Tracking u. a. nicht
 * zuverlässig nachführen.
 *
 * Änderungen in dieser Datei:
 * - Antwort auf Positionsabfrage "AZ" bzw. "EL" (optional mit \r, allein oder mit Leerzeichen)
 * - Zeilenende \r und \n als Befehlsende akzeptiert
 * - Reihenfolge der Parser-Zweige: spezielle / längere Muster vor kürzeren "AZ…"-Zweigen
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <AccelStepper.h>
#include <EEPROM.h>

#define DIR_AZ 5
#define STEP_AZ 2
#define DIR_EL 6
#define STEP_EL 3
#define EN 8

#define LEDR 13
#define LEDG 12
#define ZEROSW 11

#define SPR 200
#define RATIOEL 120
#define RATIOAZ 40

AccelStepper AZstepper(1, STEP_AZ, DIR_AZ);
AccelStepper ELstepper(1, STEP_EL, DIR_EL);

int counter;
bool done;

static void strip_cr(char *s)
{
  size_t len = strlen(s);
  while (len > 0 && (s[len - 1] == '\r' || s[len - 1] == '\n'))
  {
    s[len - 1] = '\0';
    len--;
  }
}

static bool az_el_combined_query(const char *buf)
{
  return strstr(buf, "AZ EL") != NULL || strstr(buf, "AZ  EL") != NULL;
}

static void reply_az(void)
{
  Serial.print("AZ");
  Serial.println(getAzAngle(), 1);
}

static void reply_el(void)
{
  Serial.print("EL");
  Serial.println(getElAngle(), 1);
}

void setup()
{
  AZstepper.setMaxSpeed(1000);
  AZstepper.setAcceleration(500);

  ELstepper.setMaxSpeed(2000);
  ELstepper.setAcceleration(1000);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);

  pinMode(LEDG, OUTPUT);
  digitalWrite(LEDG, LOW);
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, LOW);

  pinMode(ZEROSW, INPUT);
  digitalWrite(ZEROSW, HIGH);

  Serial.begin(19200);

  counter = 0;
  done = 0;

  setZero();
}

void loop()
{
  static int AZstep = 0;
  static int ELstep = 0;

  cmd_proc(AZstep, ELstep);
  stepper_move(AZstep, ELstep);
}

void setZero(void)
{
  while (digitalRead(ZEROSW) != LOW)
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
  static char buffer[256];
  char incomingByte;
  char data[256];

  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();
    switch (incomingByte)
    {
    case '\n':
    case '\r':
      done = 1;
      break;

    case 'U':
      done = 1;
      break;

    default:
      if (counter < (int)sizeof(buffer) - 1)
      {
        buffer[counter++] = incomingByte;
        buffer[counter] = '\0';
      }
      else
      {
        counter = 0;
        buffer[0] = '\0';
      }
      break;
    }
  }

  if (done)
  {
    strip_cr(buffer);

    if (az_el_combined_query(buffer))
    {
      Serial.print("AZ");
      Serial.print(getAzAngle(), 1);
      Serial.print(" ");
      Serial.print("EL");
      Serial.println(getElAngle(), 1);
    }
    else
    {
      for (int i = 0; i < (int)strlen(buffer); i++)
      {
        if (buffer[i] == 'R' && buffer[i + 1] == 'E' && buffer[i + 2] == 'S' &&
            buffer[i + 3] == 'E' && buffer[i + 4] == 'T')
        {
          setZero();
          stepAz = 0;
          stepEl = 0;
        }
        else if (buffer[i] == 'S' && buffer[i + 1] == 'A' && buffer[i + 2] == ' ' &&
                 buffer[i + 3] == 'S' && buffer[i + 4] == 'E')
        {
          stepAz = AZstepper.currentPosition();
          stepEl = ELstepper.currentPosition();
        }
        else if (buffer[i] == 'E' && buffer[i + 1] == 'L' &&
                 (isdigit((unsigned char)buffer[i + 2]) || buffer[i + 2] == '.' ||
                  buffer[i + 2] == '-' || buffer[i + 2] == '+'))
        {
          strncpy(data, buffer + (i + 2), sizeof(data) - 1);
          data[sizeof(data) - 1] = '\0';
          double angleEl = atof(data);
          stepEl = (deg2stepEL(angleEl)) * -1;
          Serial.println("EL");
        }
        else if (buffer[i] == 'A' && buffer[i + 1] == 'Z' &&
                 (isdigit((unsigned char)buffer[i + 2]) || buffer[i + 2] == '.' ||
                  buffer[i + 2] == '-' || buffer[i + 2] == '+'))
        {
          strncpy(data, buffer + (i + 2), sizeof(data) - 1);
          data[sizeof(data) - 1] = '\0';
          double angleAz = atof(data);
          stepAz = deg2stepAZ(angleAz);
          Serial.println("AZ");
        }
        else if (buffer[i] == 'A' && buffer[i + 1] == 'Z')
        {
          char c = buffer[i + 2];
          if (c == '\0' || c == ' ' || c == '\t')
            reply_az();
        }
        else if (buffer[i] == 'E' && buffer[i + 1] == 'L')
        {
          char c = buffer[i + 2];
          if (c == '\0' || c == ' ' || c == '\t')
            reply_el();
        }
      }
    }

    done = 0;
    counter = 0;
    buffer[0] = '\0';
  }
}

void stepper_move(int stepAz, int stepEl)
{
  AZstepper.moveTo(stepAz);
  ELstepper.moveTo(stepEl);

  AZstepper.run();
  ELstepper.run();
}

int deg2stepAZ(double deg)
{
  return (RATIOAZ * SPR * deg / 360);
}

double step2degAZ(int Step)
{
  return (360 * Step / (SPR * RATIOAZ));
}

int deg2stepEL(double deg)
{
  return (RATIOEL * SPR * deg / 360);
}

double step2degEL(int Step)
{
  return (360 * Step / (SPR * RATIOEL));
}

float getElAngle(void)
{
  return (float)step2degEL(ELstepper.currentPosition());
}

float getAzAngle(void)
{
  return (float)step2degAZ(AZstepper.currentPosition());
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
