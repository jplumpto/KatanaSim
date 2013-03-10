/* Pin Locations */


int get_breaker(int x)
{
  return states->cbStates & (1 << x);
  
} //get_breaker

void update_breakers()
{
  states->cbStates = 0;
  
  for(int i = 1; i<27 ; i++)
  {
    if ( digitalRead(CB[i]))
    {
      states->cbStates ^= 1 << i - 1;
    } //if
    
  }//for
  
} //update_breakers()

void initiate_breakers()
{
  //Set Pin locations
  CB[0] = 1;
  CB[1] = 7;
  CB[2] = 8;
  CB[3] = 9;
  CB[4] = 10;
  CB[5] = 11;
  CB[6] = 22;
  CB[7] = 23;
  CB[8] = 24;
  CB[9] = 25;
  CB[10] = 26;
  CB[11] = 27;
  CB[12] = 28;
  CB[13] = 29;
  CB[14] = 30;
  CB[15] = 31;
  CB[16] = 32;
  CB[17] = 33;
  CB[18] = 34;
  CB[19] = 35;
  CB[20] = 36;
  CB[21] = 37;
  CB[22] = 38;
  CB[23] = 39;
  CB[24] = 40;
  CB[25] = 41;
  CB[26] = 42;
  CB[27] = 43;
}
