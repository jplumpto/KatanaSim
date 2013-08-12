

//Updates state of controls
void update_controls()
{
  states->throttle = analogRead(throttlePin);
  states->propSpeed = analogRead(propSpeedPin); 
  states->pitch = analogRead(pitchPin);
  states->roll = analogRead(rollPin);
  states->yaw = analogRead(yawPin);
  states->carbHeat = analogRead(carbHeatPin);
  states->lBrake = analogRead(lBrakePin);
  states->rBrake = analogRead(rBrakePin);
  states->pBrake = analogRead(pBrakePin);
  states->choke = analogRead(chokePin);
}

//Update state of igniter and ignition position
void update_starter()
{
  int leftMag = digitalRead( leftMagPin );
  int rightMag = digitalRead( rightMagPin );
  states->ignitionPos = 0 + (rightMag == 0) + 2 * (leftMag == 0); //Matches X-Plane Dataref
  states->igniterState = digitalRead( starterPin );
}

//Updates state of switches
void update_switches()
{
  states->switchStates = SWITCHES_CLEAR;

  if( digitalRead( fuelPumpPin ) )
  {
    states->switchStates ^= SWITCHES_FUELPUMP_ON;
  } //if fuel pump
  
  if (digitalRead( strobePin ))
  {
    states->switchStates ^= SWITCHES_STROBELIGHT_ON;
  } //if 
  
  if (digitalRead( landingPin ))
  {
    states->switchStates ^= SWITCHES_LANDINGLIGHT_ON;
  } //if 
  
  if (digitalRead( taxiPin ))
  {
    states->switchStates ^= SWITCHES_TAXILIGHT_ON;
  } //if 
  
  if (digitalRead( positionPin ))
  {
    states->switchStates ^= SWITCHES_NAVLIGHT_ON;
  } //if 
  
  if (digitalRead( avionicsMasterPin ))
  {
    states->switchStates ^= SWITCHES_AVIONICSMASTER_ON;
  } //if 
  
  if (digitalRead( generatorPin ))
  {
    states->switchStates ^= SWITCHES_GENERATOR_ON;
  } //if 
  
  if (digitalRead( batteryPin ))
  {
    states->switchStates ^= SWITCHES_BATTERY_ON;
  } //if 
  
  states->trimSwitchPos = 0;
  if ( digitalRead( trimUpPin ) )
  {
    states->trimSwitchPos = 1;
  } else if ( digitalRead( trimDownPin ) )
  {
    states->trimSwitchPos = 2;
  } //if
  
  states->flapSwitchPos = -1 * digitalRead(flapsUpPin) + 1 + digitalRead(flapsFullPin);
}


//Update the speed of the fan
void update_ventilation_speed(char *buff)
{
  int val = 255; //255 is max output value
  int success = 0;
  char *entry;
  
  entry = strtok(buff,";"); //Should return 'V'
  success = sscanf(buff,"%d",&val);
  
  if (success == 1)
  {
    analogWrite(fanPin,val);
  } 
}


//Update the trim display
void update_trim_display(char *buff)
{
  int val;
  
  int success = sscanf(buff, "%d", &val);
 
  if (success == 1)
  {
    analogWrite(trimTabPin,val);
  } //if
}

//Update the flaps display
void update_flaps_display(char *buff)
{
  int val;
  
  int success = sscanf(buff, "%d", &val);
  
  if (success == 1)
  {
    analogWrite(flapsDisplayPin,val);
  } //if
}

//Update the stall warning
void update_stall_warning(char *buff)
{
  int val = 255; //255 is max output value
  int success = 0;
  char *entry;
  
//  Serial.print("Updating fan speed: ");
//  Serial.println(buff);
  
  entry = strtok(buff,";"); //Should return 'F'
  success = sscanf(buff,"%d",&val);
  
  if (success == 1)
  {
    //analogWrite(fanPin,val);
    Serial.print("Stall warning set to following percent: ");
    Serial.println(val);
  } 
}

//Update the generator annunciator warning
void update_generator_warning(char *buff)
{
  int val;
  
  success = sscanf(buff, "%d", &val);
  
  if (success == 0)
  {
    return;
  }
  
  if ( val == 1 )
  {
    digitalWrite(generatorWarningPin,HIGH);
  } //if
  else
  {
    digitalWrite(generatorWarningPin,LOW);
  }
}

//Update the fuel pressure annunciator warning
void update_fuel_pressure_warning(char *buff)
{
  int val;
  
  success = sscanf(buff, "%d", &val);
  
  if (success == 0)
  {
    return;
  }
  
  if ( val == 1 )
  {
    digitalWrite(fuelPressureWarningPin,HIGH);
  } //if
  else
  {
    digitalWrite(fuelPressureWarningPin,LOW);
  }
}
