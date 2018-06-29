//GLOBALS
uint32_t seq = 0;
bool first = true;
Data* readings = new Data();

void resetErrors(){
    /* the error code has been transmitted and so can now be reset.     
   *  The method of resetting used here allows for errors to have 
   *  occurred between sending the message and receiving
   acknowledgement. */
   if (last_transmitted_errno < last_errno && last_transmitted_errno != 0)
    last_errno = last_errno / last_transmitted_errno;
   else
    last_errno = 1;
}

void resetReadings(Data* readings){
  readings->unixtime=0;
  readings->solarBatt=0;
  readings->usage=0;
  readings->charging=0;
  readings->interrupt=0;
  readings->inactivity=0;
  readings->activity=0;
  readings->nodeBatt=0;
  readings->seq=0;
  readings->error=0;
}

void doSenseCycle()
{
  getSolarBatteryVoltage(readings);
  getBatteryVoltage(readings);
  adxl345GetInterrupt(readings);
  getLanternState(readings);
  
  if (last_errno != 1) readings->error = last_errno;  
  last_transmitted_errno = last_errno;
  
  if(hasEvent(readings))
  {
    getTime(readings);
    readings->seq = seq; 
    
    if (writeDataToFile(readings)) 
    { 
      if (first){
        nodeFunctional();
        first = false;
      }
      updateState(readings);
      resetErrors(); 
    }
    resetReadings(readings);
    seq++;
  }
}
