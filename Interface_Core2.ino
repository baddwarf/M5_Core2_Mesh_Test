#ifdef _M5Core2_H_

void menuSetup()
{
  Disbuff.createSprite(320, 240);
  DisCoverScrollbuff.createSprite(320,60);
  batteryDisplayBuff.createSprite(82,82);
  nodeDisplayBuff.createSprite(82,82);
  clockDisplayBuff.createSprite(149,82);
  Disbuff.fillRect(0,0,319,239, TFT_BLACK);
}

void showBatteryStatus()
{
  float batVoltage = M5.Axp.GetBatVoltage();
  float batPercentage = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100;
  batteryDisplayBuff.setTextDatum(TC_DATUM);
  batteryDisplayBuff.setTextColor((batPercentage <  10) ? TFT_RED : TFT_WHITE);
  batteryDisplayBuff.drawRect(0,0,81,81, TFT_WHITE);
  batteryDisplayBuff.fillRect(1,1,79,79, TFT_BLACK);
  batteryDisplayBuff.drawString(String(batPercentage) + "%",40,35);
  batteryDisplayBuff.pushSprite(2,150);
}

void showNodesCount()
{
  nodeDisplayBuff.setTextDatum(TC_DATUM);
  nodeDisplayBuff.setTextColor(TFT_WHITE);
  nodeDisplayBuff.drawRect(0,0,81,81, TFT_WHITE);
  nodeDisplayBuff.fillRect(1,1,79,79, TFT_BLACK);
  nodeDisplayBuff.drawString(String(mesh.getNodeList().size() + 1),40,35);
  nodeDisplayBuff.pushSprite(86,150); 
}

void showClock()
{
  M5.Rtc.GetTime(&RtcTime);
  String Hours = ((RtcTime.Hours > 9)? String(RtcTime.Hours) : "0" + String(RtcTime.Hours));
  String Minutes = ((RtcTime.Minutes > 9)? String(RtcTime.Minutes) : "0" + String(RtcTime.Minutes));
  String Seconds = ((RtcTime.Seconds > 9)? String(RtcTime.Seconds) : "0" + String(RtcTime.Seconds));
  String Time = Hours + ":" + Minutes + ":" + Seconds;
  clockDisplayBuff.setTextDatum(TC_DATUM);
  clockDisplayBuff.setTextColor(TFT_WHITE);
  clockDisplayBuff.drawRect(0,0,148,81, TFT_WHITE);
  clockDisplayBuff.fillRect(1,1,146,79, TFT_BLACK);
  clockDisplayBuff.drawString(Time,75,35);
  clockDisplayBuff.pushSprite(170,150); 
}

#endif
