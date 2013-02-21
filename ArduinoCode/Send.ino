

void random_test()
{
  char buff[256];
  update_controls();
  update_starter();
  update_switches();
  update_breakers();
  
  states->packetCount++;
  
  snprintf(buff,256,"Battery State: %d",states->cbStates & SWITCHES_BATTERY_ON);
  Serial.println(buff);
 
} //random_test

//Sends current state of everything
void send_states()
{
  update_controls();
  update_starter();
  update_switches();
  update_breakers();
  
  states->packetCount++;
  
  Serial.write((uint8_t*)states,nbChar);
}



//Sends to PC current state of calibration components
void send_calibration()
{
  char output[256];
  update_controls();
  //Var1:value;Var2:value;Var3:value...
  snprintf( output, 256, "PI:%d;RL:%d;YW:%d;TH:%d;LB:%d;RB:%d;ES:%d;CH:%d;CK:%d;PB:%d",states->pitch,states->roll,states->yaw,states->throttle,states->lBrake,states->rBrake,states->propSpeed,states->carbHeat,states->choke,states->pBrake );
  Serial.println( output );
}

//Sends to PC current state of all components
void send_test_data()
{
  char output[256];
  update_controls();
  update_starter();
  update_switches();

  //Var1:value;Var2:value;Var3:value...
  snprintf( output, 256, "startVar:%d;",states->startVar);
  Serial.println( output );
  snprintf( output, 256, "PI:%d;RL:%d;YW:%d;TH:%d;LB:%d;RB:%d;ES:%d;CH:%d;CK:%d;PB:%d;",states->pitch,states->roll,states->yaw,states->throttle,states->lBrake,states->rBrake,states->propSpeed,states->carbHeat,states->choke,states->pBrake );
  Serial.println( output );
  snprintf( output, 256, "Ignition:%d;Igniter:%d;",states->ignitionPos,states->igniterState );
  Serial.println( output );
  snprintSwitches( output, 256);
  Serial.println( output );
  snprintf( output, 256, "endVar:%d",states->endVar);
  Serial.println( output );
}

