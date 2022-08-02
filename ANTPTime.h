#ifndef ANTPTime_h
#define ANTPTime_h

//network time needed
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Timezone.h>

/********* NTP staff **************/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0 /* no timezone, return UTC time*/, 24*60*60*1000L /*refresh period in milliseconds - dayly*/);
TimeChangeRule uaEDT = {"UADT", Last, Sun, Mar, 3, 180};  //UTC + 3 hours
TimeChangeRule uaEST = {"UAST", Last, Sun, Oct, 4, 120};   //UTC + 2 hours
Timezone uaTimezone(uaEDT, uaEST);

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

#endif
