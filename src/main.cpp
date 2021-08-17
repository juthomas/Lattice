#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include "WiFi.h"

int vref = 1100;
#define BUTTON_1 35
#define BUTTON_2 0

#define ADC_PIN 34

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
TFT_eSPI tft = TFT_eSPI();

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



void button_init()
{
	// btn1.setClickHandler(button1_handler);
	// btn1.setLongClickHandler(button1_handler);
	// btn1.setDoubleClickHandler(button1_handler);
	// btn1.setTripleClickHandler(button1_handler);
	// btn2.setClickHandler(button2_handler);
	// btn2.setLongClickHandler(button2_handler);
	// btn2.setDoubleClickHandler(button2_handler);
	// btn2.setTripleClickHandler(button2_handler);
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
	button_init();
	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

}

void loop() {
	// put your main code here, to run repeatedly:
	showVoltage();

}