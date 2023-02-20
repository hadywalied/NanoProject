#include <Arduino.h>

#include "HX711.h"
#include <EEPROM.h>
#include "Ultrasonic.h"
// #include <U8g2lib.h>

// #ifdef U8X8_HAVE_HW_SPI
// #include <SPI.h>
// #endif
// #ifdef U8X8_HAVE_HW_I2C
// #include <Wire.h>
// #endif

// U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED

#include <SPI.h>
#include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define CALIBRATION_SWITCH 5 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// LCD PINS
//  const int LCD_PIN_1 = A4;
//  const int LCD_PIN_2 = A5;

// Known Weight def, EEPROM Index
const int KNOWN_WEIGHT = 1000;
const int IDX = 0;

HX711 scale;
Ultrasonic ultrasonic(12, 13); // An ultrasonic sensor HC-04

// Functions
void showText(String text);
void showText(String weight, String length);
void cal_setup();
void cal_loop();
void normal_setup();
void normal_loop();

int cal_Value = LOW;

void setup()
{

  // pinMode(CALIBRATION_SWITCH, INPUT_PULLUP);

  // cal_Value = digitalRead(CALIBRATION_SWITCH);

  if (cal_Value == HIGH)
  {
    // calibrate
    cal_setup();
  }
  else
  {
    // normal mode
    normal_setup();
  }
}

void loop()
{

  if (cal_Value == HIGH)
  {
    // calibrate
    cal_loop();
  }
  else
  {
    // normal mode
    //normal_loop();
    showText("ah");
  }
}

void showText(String text)
{
  // display.clearDisplay();

  // display.setTextSize(2);             // Draw 2X-scale text
  // display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  // display.setCursor(10, 15);
  // display.println(text.c_str());
  // display.display(); // Show initial text
  // delay(100);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println(text.c_str());
  delay(100);
  // Scroll to the Left
  // display.startscrollleft(0x00, 0x0F);
}

void showText(String weight, String length)
{
  // display.clearDisplay();

  // display.setTextSize(2); // Draw 2X-scale text
  // display.setTextWrap(true);
  // display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  // display.setCursor(10, 15);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println(weight.c_str());
  lcd.println(length.c_str());
  // display.println(length.c_str());
  // display.display(); // Show initial text
  delay(100);

  // Scroll to the Left
  // display.startscrollleft(0x00, 0x0F);
}

void normal_setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  Serial.begin(9600);
  // Serial.println(F("Hnaa"));

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  float *calibration_const;
  EEPROM.get(IDX, calibration_const);

  scale.set_scale(*calibration_const); // this value is obtained by calibrating the scale with known weights;

  scale.tare(); // reset the scale to 0

  // // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  // {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for (;;)
  //     ; // Don't proceed, loop forever
  // }
  // Serial.println("YAY");
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  lcd.clear();
  lcd.blink_on();
  delay(2000); // Pause for 2 seconds
  // Serial.println("YAY Again");

  // Clear the buffer
  // display.clearDisplay();

  // Draw a single pixel in white
  // showText("WOOOOOWHOOOOO");
  // Serial.println("Woo");
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  // display.display();
  // delay(2000);

  /* //SSD1306 init
  pinMode(LCD_PIN_1, OUTPUT);
  pinMode(LCD_PIN_2, OUTPUT);
  digitalWrite(LCD_PIN_1, 0);
  digitalWrite(LCD_PIN_2, 0);

  u8g2.begin(); */
}

void cal_setup()
{
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void normal_loop()
{

  /* u8g2.firstPage();
  do { */
  float weight_reading = 0.0f;
  float length_reading = 0.0f;

  if (scale.wait_ready_timeout(1000))
  {
    weight_reading = scale.get_units(5);
  }
  else
  {
    Serial.println("HX711 not found.");
    /* u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0,24, "HX711 not found."); */
    showText("HX711 not found.");
  }

  length_reading = 195.0f - ultrasonic.read(CM);

  Serial.print(weight_reading);
  Serial.print(",");
  Serial.println(length_reading);

  String shown_value = String("Weight : %f" + String(weight_reading) + "Kg" + "Length : " + String(length_reading) + "cm");
  String weight_str = "Weight: " + String(weight_reading);
  String length_str = "Length: " + String(length_reading);
  showText(weight_str, length_str);
  /* u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0,24, shown_value.c_str()); */
  delay(1500);

  // } while ( u8g2.nextPage() );
}

void cal_loop()
{
  if (scale.wait_ready_timeout(1000))
  {
    scale.set_scale();
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);

    float calibration_factor = reading / KNOWN_WEIGHT;
    Serial.print("Calibration Factor: ");
    Serial.println(calibration_factor);

    EEPROM.put(IDX, calibration_factor);
  }
  else
  {
    Serial.println("HX711 not found.");
  }

  delay(10000);
}