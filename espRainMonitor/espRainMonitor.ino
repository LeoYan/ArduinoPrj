/******************************************************************************
  Copyright (C) <2016>  <Leo Yan>
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  Contact: leo.yan.cn@gmail.com
 ******************************************************************************
  Version       : V1.0
  Author        : Leo Yan
  Created       : 2016/1
  Last Modified :
  Description   : to get forecast information from Caiyun or Openweathermap.
  Function List :
******************************************************************************/

#include "public.h"
#include <Wire.h>
#include <LiquidCrystal_SmartI2C.h>

#include "Timer.h"
#include <ESP8266WiFi.h>
#include "simpleJson.h"

#include "forecast.h"



/****/
#define WIFISSID "your ssid"
#define PASSWORD "your password"

#if 1
#define FORECASTAPIKEY "your key"
#define DOMAINNAME "api.caiyunapp.com"
#else
#define FORECASTAPIKEY "your key"
#define DOMAINNAME "api.openweathermap.org"  //"api.forecast.io"
#endif
// Location
#define LATITUDE your location
#define LONGITUDE your location


// your network SSID (name)
char ssid[] = WIFISSID;

// your network password
char pass[] = PASSWORD;

// Go to forecast.io and register for an API KEY
String forecastApiKey = FORECASTAPIKEY;

// website domain name
String webDomain = DOMAINNAME;

// Coordinates of the place you want
// weather information for
double latitude = LATITUDE;
double longitude = LONGITUDE;

#if 0
typedef struct
{
    const RainStyle_enum type;
    boolean alarmFlag;
    const char * p_icon;
}DisplayInfo_struct;

DisplayInfo_struct DisplayInfo[] =
{
    { RAIN_NONE, false, "None" },
    { RAIN_DRIZZLE, true, "Drizzle " },
    { RAIN_LIGHT, true, "Light " },
    { RAIN_MODERATE, true, "Moderate "},
    { RAIN_HEAVY, true,"Heavy" },
    { RAIN_UNKNOW, false, "unknown"},
};

#endif

typedef struct
{
    bool valid;
    String currentIcon;
    String currentSummary;
    int temperature;
    int humidity;
}DataInfo_struct;

DataInfo_struct DataInfo;

#define TRIGGER_PIN 14
#define TRIGGER_STATE HIGH

boolean isTigger = false;
int triggerTimer;
int errorCode;


WeatherClient Weather;

LiquidCrystal_I2C LCD(0x2C,16,2);
Timer T;

void setup() {

  /****/
  DataInfo.valid = false;

  Serial.begin(115200);


  /** application init **/
  pinMode(TRIGGER_PIN, INPUT);


  initModule();
  LCD.print("Instant Forecast");
  LCD.setCursor(7,1);
  LCD.print("by LeoY");

  enableWifi();

  Weather.setUnits("si");  //set SI unit, Such as: Degrees Celsius

  T.every((5*60000), getDataFromWifi);

  /****/
  getDataFromWifi();

  isTigger = true;


}



void loop() {

    if ( isTigger )
    {
        isTigger = false;
        
        displayInfo();

        T.after(30000, overtime);
    }

    T.update();

}


void initModule()
{
    /****/
    LCD.init(5, 4);
    LCD.setContrast(56);
    LCD.backlight();
    LCD.noBlink();
}


void displayInfo( )
{
    uint8_t i;

    LCD.clear();
    //LCD.noBlink();

    if ( DataInfo.valid )
    {
        LCD.setCursor( 1, 0 );
        //LCD.print(DataInfo.currentSummary.c_str());
        LCD.print(DataInfo.currentIcon.c_str());

        LCD.setCursor( 0, 1 );
        LCD.print( "T: " );
        LCD.print( DataInfo.temperature) ;
        LCD.print(" C  " );

        LCD.print( "RH: " );
        LCD.print( DataInfo.humidity) ;
        LCD.print("%" );

        if ( String("RAIN") == DataInfo.currentIcon
            || String("SLEET") == DataInfo.currentIcon
        || String("FOG") == DataInfo.currentIcon
        || String("HAZE") == DataInfo.currentIcon
        || String("SNOW") == DataInfo.currentIcon )
        {
            LCD.backlight();
            LCD.setCursor(0, 0);
            LCD.blink();
        }
    }
    else
    {
        LCD.print("Sorry, No data!");
        LCD.setCursor(0, 1);
        LCD.print(errorCode);
        LCD.backlight();
    }


}


void overtime()
{
    LCD.noBacklight();
    LCD.noBlink();
    triggerTimer = T.every(100, checkTrigger);
}

void checkTrigger()
{
    static uint8_t count = 0;
    Serial.print(digitalRead(TRIGGER_PIN));

    if ( TRIGGER_STATE == digitalRead(TRIGGER_PIN) )
    {
        count++;
    }

    if ( count > 2 )
    {
        isTigger = true;
        count = 0;
        T.stop(triggerTimer);
    }
}




void getDataFromWifi()
{
    boolean result;

    DataInfo.valid = false;

    /**confirm wifi connectted **/
    result = true;
    if ( WiFi.status() != WL_CONNECTED )
    {
    	result = enableWifi();
      errorCode = result ? 0 : 1;
      
    }

    /**get forcast**/
    if ( result )
    {
		/**get data**/
    	result = Weather.updateWeatherData(webDomain, forecastApiKey, latitude, longitude);

    	if ( result )
    	{
    		DataInfo.temperature = Weather.getCurrentTemp();
    		DataInfo.humidity = Weather.getCurrentHumidity();
  			DataInfo.currentIcon = Weather.getCurrentIcon();
  			DataInfo.currentSummary = Weather.getCurrentSummary();
  			DataInfo.valid = true;
    	}

      errorCode = result ? 0 : 2;

    }

}


bool enableWifi( )
{
	   bool ret;
    uint8_t counter;

    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

	   ret = true;
    counter = 0;
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");

      counter++;
      if ( counter > 50 )
      {
    	  ret = false;
    	  break;
      }
    }

    if (ret)
    {
		Serial.println("");

		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());
    }

    return ret;

}


