
//network time needed
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <Dusk2Dawn.h>

/********* NTP staff **************/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0 /* no timezone, return UTC time*/, 24*60*60*1000L /*refresh period in milliseconds - dayly*/);
TimeChangeRule uaEDT = {"UADT", Last, Sun, Mar, 3, 180};  //UTC + 3 hours
TimeChangeRule uaEST = {"UAST", Last, Sun, Oct, 4, 120};   //UTC + 2 hours
Timezone uaTimezone(uaEDT, uaEST);
Dusk2Dawn deviceLocation(49.842957, 24.031111, 2);//Lviv latitude and longtitude and timezone

time_t getNow(void){
  timeClient.update();
  unsigned long rawTime = timeClient.getEpochTime();
  if (year(rawTime) < 2020) return 0; 
  rawTime = uaTimezone.toLocal(rawTime);
  return rawTime;
}

String getTimeAsPath(void) {
  timeClient.update();
  unsigned long rawTime = timeClient.getEpochTime();
  if (year(rawTime) < 2020) return String(); //something wrong with time servers
  rawTime = uaTimezone.toLocal(rawTime);
  char str[20];
  sprintf(str, "%04u/%02u/%02u/%02u/%02u", year(rawTime), month(rawTime), day(rawTime), hour(rawTime), minute(rawTime));
//  Serial.printf("getTimeAsPath = %s\n", str);
  
//  struct tm * timeinfo;
//  timeinfo = localtime (&rawTime);
//  strftime(str, 20, "%Y/%m/%d/%H/%M", timeinfo);
//  Serial.printf("time 1 %s\n", str);

  String result = str;
  return result;
}

time_t getSunriseTime(time_t t){
  //input and out parameters are in correct timezone and tuned due to daylight saving configuration
  //it will return different time for same day due to uaTimezone.locIsDST(t) will return different values for 01:00 and for 06:00 for day where clock is tuned
  int sunrise  = deviceLocation.sunrise(year(t), month(t), day(t), uaTimezone.locIsDST(t));
  tmElements_t tm;
  breakTime(t, tm);
  tm.Hour = sunrise / 60;
  tm.Minute = sunrise % 60;
  return makeTime(tm); 
}

time_t getSunsetTime(time_t t){
  //input and out parameters are in correct timezone and tuned due to daylight saving configuration
  int sunset  = deviceLocation.sunset(year(t), month(t), day(t), uaTimezone.locIsDST(t));
  tmElements_t tm;
  breakTime(t, tm);
  tm.Hour = sunset / 60;
  tm.Minute = sunset % 60;
  return makeTime(tm); 
}

