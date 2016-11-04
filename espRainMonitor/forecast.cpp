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


#include "forecast.h"
#include "simpleJson.h"
#include <ESP8266WiFi.h>

bool WeatherClient::updateWeatherData(String domainName, String apiKey, double lat, double lon) {
  bool result;
  WiFiClient client;
  const int httpPort = 80;
  uint8_t count;

  newDataFlag = false;

  Serial.print("Connecting to server: ");
  Serial.println(domainName);

  // connnect to our server
  count = 0;
  while (!client.connect(domainName.c_str(), httpPort)) {
    Serial.println("connection failed, retrying...");
    count++;
    if ( count > 10 )
    {
    	return false;
    }
    delay(3000);
  }

#if 1
  //https://api.caiyunapp.com/v2/hDa1LTmgISwY5Zzm/121.45,32.23/realtime.json??lang=en

  String url = "https://" + domainName + "/v2/" + apiKey + "/" + String(lon) + "," + String(lat) + "/realtime.json?lang=en";

#else
  //http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&units=metric&APPID={APIKEY}

  String url = "http://" + domainName + "/data/2.5/weather?lat=" + String(lat) + "&" + "lon=" + String(lon) + "&units=metric&APPID=" + apiKey;
#endif
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host:" + domainName +
               "\r\nConnection: close\r\n\r\n");

  // wait for response from server
//  count = 0;
//  while(!client.available())
//  {
//	count++;
//	if ( count > 50 )
//	{
//		return false;
//	}
//
//    delay(100);
//  }


  /**get Json data**/
  if ( LightJson.start( client ) )
  {

	  uint8_t curLevel;
	  uint8_t arrayLevel;
	  JsonItem_struct JsonItem;

	  /**intial data**/
	  String fatherName;

	  do
	  {
		  result = LightJson.getItem( JsonItem, curLevel );

		  if (result)
		  {

			  if (0 == curLevel)
			  {
				  break;
			  }

#if 1
			  //{"status":"ok","lang":"zh_CN","server_time":1474341000,"tzshift":28800,"location":[32.23,121.45],"unit":"metric","result":{"status":"ok","temperature":24.0,"skycon":"CLEAR_DAY","cloudrate":0.0,"aqi":10.0,"humidity":0.69,"pm25":10.0,"precipitation":{"nearest":{"status":"ok","distance":25.02,"intensity":0.1875},"local":{"status":"ok","intensity":0.0,"datasource":"radar"}},"wind":{"direction":6.54,"speed":21.05}}}

			  switch (JsonItem.data.type)
			  {
				  case JSON_TYPE_Int:
					  break;

				  case JSON_TYPE_Float:
					  if ( String("temperature") == JsonItem.name )
					  {
						  currentTemp = JsonItem.data.valuefloat;
					  }
					  else if ( String("humidity") == JsonItem.name )
					  {
						  currentHumidity = JsonItem.data.valuefloat*100;
					  }
					  else if ( String("intensity") == JsonItem.name )
					  {
						  if ( String("local") == fatherName )
						  {
							  currentPrecipitationIntensity = JsonItem.data.valuefloat;
						  }
					  }

					  break;

				  case JSON_TYPE_String:
					  if (String("description") == JsonItem.name)
					  {
						  currentSummary = JsonItem.data.valueStr;
					  }
					  else if (String("skycon") == JsonItem.name)
					  {
						  currentIcon = JsonItem.data.valueStr;
					  }

					  break;

				  case JSON_TYPE_Array:
					  fatherName = JsonItem.name;
					  break;

				  case JSON_TYPE_Object:
					  fatherName = JsonItem.name;
					  break;

				  default:
					  ;  //nothing

			  }



#else
			  switch (JsonItem.data.type)
			  {
				  case JSON_TYPE_Int:
					  break;

				  case JSON_TYPE_Float:
					  if ( String("temp") == JsonItem.name )
					  {
						  currentTemp = JsonItem.data.valuefloat;
					  }
					  else if ( String("humidity") == JsonItem.name )
					  {
						  currentHumidity = JsonItem.data.valuefloat;
					  }

					  break;

				  case JSON_TYPE_String:
					  if (String("description") == JsonItem.name)
					  {
						  currentSummary = JsonItem.data.valueStr;
              Serial.println(currentSummary.c_str());
					  }
					  else if (String("main") == JsonItem.name)
					  {
						  currentIcon = JsonItem.data.valueStr;
					  }

					  break;

				  case JSON_TYPE_Array:

					  break;

				  default:
					  ;  //nothing

			  }


#endif
		  }
		  else
		  {
			  //nothing
		  }

	  }while(result);
  }

  LightJson.stop();

  Serial.println();
  Serial.println("closing connection");

  newDataFlag = true;

  return true;
}

void WeatherClient::setUnits(String units) {
   myUnits = units;
}

String WeatherClient::getKey(String line) {
  int separatorPosition = line.indexOf("=");
  if (separatorPosition == -1) {
    return "";
  }
  return line.substring(0, separatorPosition);
}

String WeatherClient::getValue(String line) {
  int separatorPosition = line.indexOf("=");
  if (separatorPosition == -1) {
    return "";
  }
  return line.substring(separatorPosition + 1);
}

int WeatherClient::getCurrentTemp(void) {
  return currentTemp;
}
int WeatherClient::getCurrentHumidity(void) {
  return currentHumidity;
}

//icon: A machine-readable text summary of this data point, suitable for selecting an icon for display. If defined, this property will have one of the following values: clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, or partly-cloudy-night. (Developers should ensure that a sensible default is defined, as additional values, such as hail, thunderstorm, or tornado, may be defined in the future.)
String WeatherClient::getCurrentIcon(void) {
  return currentIcon;
}

//summary: A human-readable text summary of this data point. (Do not use this value for automated purposes: you should use the icon property, instead.)
String WeatherClient::getCurrentSummary(void) {
  return currentSummary;
}
String WeatherClient::getIconToday(void) {
  return iconToday;
}
int WeatherClient::getMaxTempToday(void) {
  return maxTempToday;
}
int WeatherClient::getMinTempToday(void) {
  return minTempToday;
}
String WeatherClient::getSummaryToday(void) {
  return summaryToday;
}
int WeatherClient::getMaxTempTomorrow(void) {
  return maxTempTomorrow;
}
int WeatherClient::getMinTempTomorrow(void) {
  return minTempTomorrow;
}
String WeatherClient::getIconTomorrow(void) {
  return iconTomorrow;
}
String WeatherClient::getSummaryTomorrow(void) {
  return summaryTomorrow;
}
