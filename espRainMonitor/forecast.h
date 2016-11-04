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

#ifndef FORECAST_H_
#define FORECAST_H_

#include "public.h"
#include <Arduino.h>

class WeatherClient {
  private:
    bool newDataFlag;
    float currentTemp;
    float currentHumidity;
    float currentPrecipitationIntensity;
    String currentIcon;
    String currentSummary;
    String iconToday;
    int maxTempToday;
    int minTempToday;
    String summaryToday;
    int maxTempTomorrow;
    int minTempTomorrow;
    String iconTomorrow;
    String summaryTomorrow;
    String apiKey;
    String domainName;
    String myUnits = "us";
    String myLanguage;

    String getValue(String line);
    String getKey(String line);

  public:
    WeatherClient(){ newDataFlag = false; };
    bool updateWeatherData(String domainName, String apiKey, double lat, double lon);
    inline bool isNewData() { return newDataFlag; };
    void setUnits(String units);
    int getCurrentTemp(void);
    int getCurrentHumidity(void);
    String getCurrentIcon(void);
    String getCurrentSummary(void);
    String getIconToday(void);
    int getMaxTempToday(void);
    int getMinTempToday(void);
    String getSummaryToday(void);
    int getMaxTempTomorrow(void);
    int getMinTempTomorrow(void);
    String getIconTomorrow(void);
    String getSummaryTomorrow(void);
    float getCurrentPrecipitationIntensity(void) { return currentPrecipitationIntensity;}

};


#endif /* FORECAST_H_ */
