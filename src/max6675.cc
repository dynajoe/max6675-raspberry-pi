#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif
#include <node.h>
#include "max6675.h"
extern "C" {
#include <wiringPi.h>
}
using namespace v8;

Persistent<Function> Max6675::constructor;

Max6675::Max6675(int miso, int ss, int clk) :
  m_misoPin(miso),
  m_ssPin(ss),
  m_clkPin(clk) 
{

  pinMode(m_ssPin, OUTPUT);
  pinMode(m_clkPin, OUTPUT); 
  pinMode(m_misoPin, INPUT);

  digitalWrite(m_ssPin, HIGH);
}

Max6675::~Max6675() 
{
}

void Max6675::Init() 
{
  if (wiringPiSetup() == -1) {
    ThrowException(Exception::TypeError(String::New("Unable to setup wiringPi!")));
    return;
  }

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  
  tpl->SetClassName(String::NewSymbol("Max6675"));
  
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  tpl->PrototypeTemplate()->Set(String::NewSymbol("read"),
    FunctionTemplate::New(ReadTemperature)->GetFunction());
  
  constructor = Persistent<Function>::New(tpl->GetFunction());
}

Handle<Value> Max6675::New(const Arguments& args) 
{
  HandleScope scope;

  Handle<Object> options = Handle<Object>::Cast(args[0]);
  
  int misoPin = options->Get(String::New("miso"))->ToInt32()->Value();
  int ssPin = options->Get(String::New("ss"))->ToInt32()->Value();
  int clkPin = options->Get(String::New("clk"))->ToInt32()->Value();

  if (args.IsConstructCall()) {
    Max6675* obj = new Max6675(misoPin, ssPin, clkPin);
    obj->Wrap(args.This());
    return args.This();
  } else {
    const unsigned argc = 1;
    Local<Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }
}

Handle<Value> Max6675::NewInstance(const Arguments& args) 
{
  HandleScope scope;

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}

Handle<Value> Max6675::ReadTemperature(const Arguments& args) 
{
  HandleScope scope;
  Max6675* obj = ObjectWrap::Unwrap<Max6675>(args.This());
  float result = obj->Read();
  return scope.Close(Number::New(result));
}

float Max6675::Read() 
{
   int value = 0;
   int error_tc = 0;
   float temp = 0.0;

   /* 
   Initiate a temperature conversion. According to MAX's tech notes FAQ's 
   for the chip, Line going high initiates a conversion, which means, we 
   need to clock the chip low to high to initiate the conversion, then wait 
   for the conversion to be complete before trying to read the data from 
   the chip.
   */
   digitalWrite(m_ssPin,LOW);                                 
   delay(2);
   digitalWrite(m_ssPin,HIGH);
   delay(50);

   /* Read the chip and return the raw temperature value */

   /*
   Bring CS pin low to allow us to read the data from
   the conversion process
   */
   digitalWrite(m_ssPin,LOW);

   /* Cycle the clock for dummy bit 15 */
   digitalWrite(m_clkPin,HIGH);
   delay(1);
   digitalWrite(m_clkPin,LOW);

   /* 
   Read bits 14-3 from MAX6675 for the Temp. Loop for each bit reading 
   the value and storing the final value in 'temp' 
   */
   int i = 0;
   for (i=11; i>=0; i--) {
   digitalWrite(m_clkPin,HIGH);
   value += digitalRead(m_misoPin) << i;
   digitalWrite(m_clkPin,LOW);
   }

   /* Read the TC Input inp to check for TC Errors */
   digitalWrite(m_clkPin,HIGH);
   error_tc = digitalRead(m_misoPin);
   digitalWrite(m_clkPin,LOW);

   /* 
   Read the last two bits from the chip, faliure to do so will result 
   in erratic readings from the chip. 
   */

   for (i=1; i>=0; i--) {
   digitalWrite(m_clkPin,HIGH);
   delay(1);
   digitalWrite(m_clkPin,LOW);
   }

   // Disable Device
   digitalWrite(m_ssPin, HIGH);

   /* 
   Keep in mind that the temp that was just read is on the digital scale
   from 0C to 1023.75C at a resolution of 2^12.  We now need to convert
   to an actual readable temperature (this drove me nuts until I figured 
   this out!).  Now multiply by 0.25.  I tried to avoid float math but
   it is tough to do a good conversion to F.  THe final value is converted 
   to an int and returned at x10 power.

   2 = temp in deg F
   1 = temp in deg C
   0 = raw chip value 0-4095
   */

   temp = (value*0.25) * 9.0/5.0 + 32.0;

   /* Output negative of CS_pin if there is a TC error, otherwise return 'temp' */
   if(error_tc != 0) {
      return -m_ssPin; 
   } else { 
      return temp; 
   }
}
