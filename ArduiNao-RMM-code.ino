/***************************************
* Bot4You-NAO Eyebrows
* v1.0
* Date: Jun 2015
* Programmer: Hoang-Long Cao
****************************************/
#include <Servo.h> 
// pwm
int servo_L_pin = 11;
int servo_R_pin = 12;
String pwm1, pwm2;
Servo servo_L;
Servo servo_R;
int neutral = 90;
                  
int nCpt = 0;
unsigned long nLastOutputMs = 0;

const int nSizeSerialInputMax = 512; // a bit limited, but...
char bufSerialInput[nSizeSerialInputMax+1];
int nSizeSerialInput = 0;
long int anReceivedValues[16]; // max is 16 value to receive
void setup()
{
  int i;
  servo_L.attach(servo_L_pin);
  servo_R.attach(servo_R_pin);
  servo_L.write(neutral);
  servo_R.write(neutral);
  
  Serial.begin(115200);

}

 
// extract data from ascii buffer, and store it to pDataToFilled
// return the number of found data
// CURRENTLY: handles only numbers !!! 
int extractValues( const char * buf, int nLenToAnalyse, long int * pDataToFilled )
{
  Serial.print( "#extractValues: '" );
  Serial.print( buf );
  Serial.print( "', nLenToAnalyse:" );
  Serial.println( nLenToAnalyse );
  int nCur = 0;
  if( buf[nCur] == ' ' )
  {
    for( ; buf[nCur] == ' '; ++nCur ); // skip spaces
  }
  if( buf[nCur] == '[' )  
  {
     // array of variables
     ++nCur;
     int nNbrVariable = 0;
     while( 1 )
     {
        pDataToFilled[nNbrVariable] = atoi( &buf[nCur] );
//        Serial.print( "# nNbrVariable:" );
//        Serial.print( nNbrVariable );        
//        Serial.print( ", pDataToFilled[nNbrVariable]: " );
//        Serial.println( pDataToFilled[nNbrVariable] );
        
        ++nNbrVariable;
        for( ; buf[nCur] != ','; ++nCur ) // jump to next ','
        {
          if( buf[nCur] == ']' || buf[nCur] == '\0'|| nCur >= nLenToAnalyse )
          {
            break;
          }
        }
        if( buf[nCur] != ',' )
        {
          break;
        }
        ++nCur; // point next value after ','
     }
     return nNbrVariable;
  }
  // just one variable
  //((char*)buf)[nCur+nLenToAnalyse] = '\0'; // pour tester, mais atoi n'est pas dérangé en fait!
  pDataToFilled[0] = atoi( &buf[nCur] );
  return 1;
}

//
// analyse received buffer and update variable
//
//    receive data in a specific way:
//        ["dataname",value]\n  a pair [data,value], the value can be an integer, a float, a string, an array, an array of int, value, string, array...
//        #blablabla\n            a comments, it will only been printed in the console.
//        \n can be replaced by the two char: RN
//
//
void analyseReceivedData( char * buf )
{
  if( buf[0] == '#' )
  {
    // just a comments, skipping
    return;
  }
  int nLen;
  for( nLen = 0; buf[nLen]; ++nLen );
  if( buf[0] == '[' )
  {
     // some data in the form ["data",value]
    const int nPosDataName = 2;     
    int nLenDataName;
    int nPosComma;
    int nNbrData;
    for( nPosComma = 0; nPosComma < nLen && buf[nPosComma] != ','; ++nPosComma );
    if( nPosComma >= nLen )
    {
      return;
    }
    nLenDataName = nPosComma - 1 - nPosDataName;
    nNbrData = extractValues( &buf[nPosComma+1], nLen - nLenDataName - 5, anReceivedValues );
    Serial.print( "# nNbrData:" );
    Serial.println( nNbrData );
    const char * pDataBegin = &buf[nPosDataName];
    if( strncmp( pDataBegin, "PositionL", nLenDataName ) == 0 && nNbrData >= 1 )
    {
      Serial.print( "#PositionL info received:" );
      Serial.println( anReceivedValues[0] );
      servo_L.write(anReceivedValues[0]);
      servo_R.write(180-anReceivedValues[0]);
    }
//    if( strncmp( pDataBegin, "PositionR", nLenDataName ) == 0 && nNbrData >= 1 )
//    {
//      Serial.print( "#PositionR info received:" );
//      Serial.println( anReceivedValues[0] );
//      servo_R.write(anReceivedValues[0]);
//    }
//     
  }
}

int bPreviousWasR = false; // help to wait for \r\n
void handlesSerialInput()
{
  if( Serial.available() <= 0 )
  {
    return;
  }
  // read the incoming byte:
  int nIncomingByte = Serial.read();
  // say what you got:
//  Serial.print("# I received: ");
//  Serial.println(nIncomingByte, DEC);
  int bEndOfInput = false;
  if( nSizeSerialInput >= nSizeSerialInputMax )
  {
    Serial.println( "#Too much char in input => shortened" );
    bEndOfInput = true;
  }
  
  if( nIncomingByte == '\r' || nIncomingByte == 'R' )
  {
    bPreviousWasR = true;
  }
  else if( ( nIncomingByte == '\n' || nIncomingByte == 'N' ) && bPreviousWasR )
  {
    bEndOfInput = true;
  }
  else
  {
    bPreviousWasR = false;
    bufSerialInput[nSizeSerialInput] = nIncomingByte;
    ++nSizeSerialInput;    
  }
   
  if( bEndOfInput )
  {
    bufSerialInput[nSizeSerialInput] = '\0';
    Serial.print("# I received total: ");
    Serial.println(bufSerialInput);
    analyseReceivedData( bufSerialInput );
    bPreviousWasR = false;
    nSizeSerialInput = 0; // rst
  }
}

/*
* loop() – this function will start after setup
* finishes and then repeat
*/
void loop()
{
  nCpt += 1;
  // if( nCpt > 300 )
  if( millis() - nLastOutputMs > 1000 )
  {
    Serial.println( "[\"Ping\",1]" );
    
    nCpt = 0;
    nLastOutputMs = millis();   
  }

  handlesSerialInput();
}
