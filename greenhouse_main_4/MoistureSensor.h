#ifndef MoistureSensor_H_
#define MoistureSensor_H_
#include "Arduino.h"
/*------------------------------------------------------// 
Moisture sensors.
*/

class MoistureSensor {
 public:
  //Declaring function below with all its variables.
  int moistureRead(int moistureSensor, bool moistureDry, bool moistureWet);
 private:

};



#endif  /* MoistureSensor_H_ */
