
//Import SD card library and define variables
#include <SdFat.h>
SdFat SD;
  
char fileName[10] = "SUM_" NODE_ID ".csv";
  
SdFile file;

// TODO this should be a method of Data
void _write(Data* reading){ 
  file.print(reading->unixtime);
  file.print(",");
  file.print(reading->tempThermocouple);
  file.print(",");
  file.print(reading->tempSi7021);
  file.print(",");
  file.print(reading->humidity);
  file.print(",");
  file.print(reading->nodeBatt);
  file.print(",");
  file.print(reading->error);
  file.print(",");
  file.println(reading->seq);
}
  
/**
 * Configure the SD card for use. This function checks to
 * see if the card is present and can be initialized:
 */
void setupSD() {
  dbg("Setting up SD");
    
  if (!SD.begin(SD_CS_PIN, SPI_HALF_SPEED)) {
    dbg("SD card failed, or not present");
    return; //Need to turn LED on or similar
  }
  
  dbg(fileName);
  
  dbg("SD card initialized");
}
  
/**
 * Logs a string to a datafile
 * @param pointer to a data struct
 * @return True if the String was saved
 */
bool writeDataToFile(Data* reading)
{
  dbg("SD: Write start");
  if (!file.open(fileName, O_APPEND | O_CREAT | O_WRITE )) {
    dbg("SD card failed, or not present");
    reportError(ERR_CSV_OPEN);
    return false;
  }
    
  _write(reading);
  
  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.close() || file.getWriteError()) {
    dbg("SD: Write error");
    reportError(ERR_CSV_WRITE);
    return false;
  }
  dbg("SD: Write end");
  return true;
}
