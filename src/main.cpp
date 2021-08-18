#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include "WiFi.h"
#include <ESP32Servo.h>

int vref = 1100;
#define BUTTON_1 35
#define BUTTON_2 0

#define ADC_PIN 34

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
TFT_eSPI tft = TFT_eSPI();
ESP32PWM pwm;
Servo servos[3];
uint16_t current_servo = 0;
//FLEX 1 MID : 1575
//FLEX 2 MID : 1837
//MIN 50, 160;
uint16_t positions[3] = {90, 90, 90};
#define FLEX1 33
#define FLEX2 32


double fmap(double x, double in_min, double in_max, double out_min, double out_max) {
		const double dividend = out_max - out_min;
		const double divisor = in_max - in_min;
		const double delta = x - in_min;
		if(divisor == 0){
				log_e("Invalid map input range, min == max");
				return -1; //AVR returns -1, SAM returns 0
		}
		return (delta * dividend + (divisor / 2.0)) / divisor + out_min;
}


/**
 * 
 * Fonction appellee quand le bouton 1 est appuyé
 * 
 */
void	button1_handler(Button2 &btn)
{
	//Type de clic
	//(SINGLE_CLICK, DOUBLE_CLICK, TRIPLE_CLICK, LONG_CLICK)
	uint32_t click_type = btn.getClickType();
	if (click_type == DOUBLE_CLICK)
	{
		current_servo = (current_servo + 1) % 3;
	}
	else
	{
		positions[current_servo] = positions[current_servo] + 10 > 180 ? 180 : positions[current_servo] + 10;
		// positions[current_servo] = (positions[current_servo] + 10) % 180;
	}

}

/**
 * 
 * Fonction appellee quand le bouton 1 est appuyé
 * 
 */
void	button2_handler(Button2 &btn)
{
	//Type de clic
	//(SINGLE_CLICK, DOUBLE_CLICK, TRIPLE_CLICK, LONG_CLICK)
	uint32_t click_type = btn.getClickType();
	if (click_type == DOUBLE_CLICK)
	{
		current_servo = (current_servo + 2) % 3;
	}
	else
	{
		positions[current_servo] = positions[current_servo] - 10 < 0 ? 0 : positions[current_servo] - 10;
		// positions[current_servo] = (positions[current_servo] + 170) % 180;
	}
}

void button_init()
{
	btn1.setClickHandler(button1_handler);
	btn1.setLongClickHandler(button1_handler);
	btn1.setDoubleClickHandler(button1_handler);
	btn1.setTripleClickHandler(button1_handler);
	btn2.setClickHandler(button2_handler);
	btn2.setLongClickHandler(button2_handler);
	btn2.setDoubleClickHandler(button2_handler);
	btn2.setTripleClickHandler(button2_handler);
}

void button_loop()
{
	btn1.loop();
	btn2.loop();
}

void showVoltage()
{
	static uint64_t timeStamp = 0;
	if (millis() - timeStamp > 1000)
	{
		timeStamp = millis();
		uint16_t v = analogRead(ADC_PIN);
		float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
		String voltage = "Voltage :" + String(battery_voltage) + "V";
		Serial.println(voltage);
		tft.fillScreen(TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(voltage, tft.width() / 2, tft.height() / 2);
	}
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servos[0].setPeriodHertz(50);
	servos[1].setPeriodHertz(50);
	servos[2].setPeriodHertz(50);

	servos[0].attach(25);
	servos[1].attach(26);
	servos[2].attach(27);

	servos[0].write(positions[0]);
	servos[1].write(positions[1]);
	servos[2].write(positions[2]);



	button_init();
	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	tft.setTextSize(1);
	tft.setTextColor(TFT_WHITE);
	tft.setCursor(0, 0);
	tft.setTextDatum(MC_DATUM);

}

void drawBatteryLevel(TFT_eSprite *sprite, int x, int y, float voltage)
{
	uint32_t color1 = TFT_GREEN;
	uint32_t color2 = TFT_WHITE;
	uint32_t color3 = TFT_BLUE;
	uint32_t color4 = TFT_RED;

	if (voltage > 4.33)
	{
		(*sprite).fillRect(x, y, 30, 10, color3);
	}
	else if (voltage > 3.2)
	{
		(*sprite).fillRect(x, y, map((long)(voltage * 100), 320, 430, 0, 30), 10, color1);
		(*sprite).setCursor(x + 7, y + 1);
		(*sprite).setTextColor(TFT_DARKGREY);
		(*sprite).printf("%02ld%%", map((long)(voltage * 100), 320, 432, 0, 100));
	}
	else
	{
		(*sprite).fillRect(x, y, 30, 10, color4);
	}

	(*sprite).drawRect(x, y, 30, 10, color2);
}

void oldloop()
{
	// put your main code here, to run repeatedly:
	static int16_t servo_position = 0;
	static int16_t sens = 1;
	// showVoltage();
	button_loop();

	// servos[0].write(servo_position);
	// servos[1].write(servo_position);
	// servos[2].write(servo_position);
	TFT_eSprite drawing_sprite = TFT_eSprite(&tft);
	drawing_sprite.setColorDepth(8);
	drawing_sprite.createSprite(tft.width(), tft.height());
	uint16_t v = analogRead(ADC_PIN);
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
	drawing_sprite.setTextSize(1);
	drawing_sprite.setTextFont(1);
	drawing_sprite.setTextDatum(MC_DATUM);
	drawing_sprite.setCursor(0, 0);
	drawing_sprite.fillSprite(TFT_BLACK);
	drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);

	if (current_servo == 0)
		drawing_sprite.setTextColor(TFT_GREEN);
	else
		drawing_sprite.setTextColor(TFT_RED);

	drawing_sprite.printf("Servo[0] : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", positions[0]);

	if (current_servo == 1)
		drawing_sprite.setTextColor(TFT_GREEN);
	else
		drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Servo[1] : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", positions[1]);

	if (current_servo == 2)
		drawing_sprite.setTextColor(TFT_GREEN);
	else
		drawing_sprite.setTextColor(TFT_RED);

	drawing_sprite.printf("Servo[2] : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", positions[2]);

	drawing_sprite.printf("\n\n");

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Flex 1 : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", analogRead(FLEX1));

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Flex 2 : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", analogRead(FLEX2));

	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();

	servos[current_servo].write(positions[current_servo]);
}

void loop() {
	oldloop();
	button_loop();

	delay(1);
}