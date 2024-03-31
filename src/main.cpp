#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int lcdColumns = 16;
int lcdRows = 2;
int buttonPin = 19;
int gas_sensor = 34;
int co_sensor = 35;
float m = -0.353;
float c = 0.711;
float R0 = 23.30;
float m1 = -0.67;
float c1 = 1.34;
float R01 = 5.80;
int last_state = HIGH;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

float calculatePPM(int sensorValue, float R0, float m, float c);

void setup()
{
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  pinMode(gas_sensor, INPUT);
  pinMode(co_sensor, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop()
{
    float ppm_gas = calculatePPM(analogRead(gas_sensor), R0, m, c);
    float ppm_co = calculatePPM(analogRead(co_sensor), R01, m1, c1);
    int button_state = digitalRead(buttonPin);

    if (button_state == LOW)
    {
        if (last_state == HIGH)
            lcd.clear();
        // set cursor to first column, first row
        lcd.setCursor(0, 0);
        // print message
        lcd.print("Gas: ");
        lcd.print(ppm_gas);

        // set cursor to first column, second row
        lcd.setCursor(0,1);
        lcd.print("CO: ");
        lcd.print(ppm_co);
    }
    else
    {
        if (last_state == LOW)
            lcd.clear();
        lcd.setCursor(0, 0);
        if (ppm_gas > 60) 
            lcd.print("Gas level: Bad");
        else
            lcd.print("Gas level: Good");

        lcd.setCursor(0,1);
        if (ppm_co > 2) 
            lcd.print("CO level: Bad");
        else
            lcd.print("CO level: Good");
    }

    last_state = button_state;

    delay(350);
}

// calculate concentration of gas in parts per million
// params: raw sensor reading, resistance of the sensor in clean air, slope of the calibration curve, intercept of the calibration curve
float calculatePPM(int sensorValue, float R0, float m, float c)
{
    float sensor_volt = sensorValue * (3.3 / 4096.0);
    float RS_gas = ((3.3 * 10.0) / sensor_volt) - 10.0;
    float ratio = RS_gas / R0;
    double ppm_log = (log10(ratio) - c) / m;
    double ppm = pow(10, ppm_log);
    return ppm;
}
