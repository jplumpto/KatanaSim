
// Update buffer with string of updated switchs
void snprintSwitches(char *buff, int maxChars)
{
  uint32_t cb = states->cbStates;
  snprintf( buff, maxChars, "Fuel:%d;Strobe:%d;Land:%d;Taxi:%d;Position:%d;Avionics:%d;Gen:%d;Batt:%d;",
            cb & SWITCHES_FUELPUMP_ON , cb & SWITCHES_STROBELIGHT_ON , cb & SWITCHES_LANDINGLIGHT_ON ,
            cb & SWITCHES_TAXILIGHT_ON , cb & SWITCHES_NAVLIGHT_ON , cb & SWITCHES_AVIONICSMASTER_ON ,
            cb & SWITCHES_GENERATOR_ON , cb & SWITCHES_BATTERY_ON );
  
} //snprintSwitches


//Initialize State variable
void * create_state(){
  nbChar = sizeof(ArduinoStates);
  states = (ArduinoStates *) malloc(nbChar);

  if (states)
  {
      states->startVar = 0xAAA;
      states->packetCount = 0;
      
      states->throttle = 0;
      states->propSpeed = 0; 
      states->pitch = 0;
      states->roll = 0;
      states->yaw = 0;
      states->carbHeat = 0;
      states->lBrake = 0;
      states->rBrake = 0;
      states->choke = 0;
      states->pBrake = 0;
      
      states->switchStates = 0;
      states->trimSwitchPos = 0;
      states->flapSwitchPos = 0;
      
      states->cbStates = 0;
      
      states->endVar = 0xFFF;
  }
  
}
