#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif
#ifndef MAX6675_H
#define MAX6675_H

#include <node.h>

using namespace v8;

class Max6675 : node::ObjectWrap {
   public:
      static void Init();
      static Handle<Value> NewInstance(const Arguments& args);
      static Handle<Value> ReadTemperature(const Arguments& args);
      float Read();

   private:
      Max6675(int miso, int ss, int clk);
      ~Max6675();

      static Handle<Value> New(const Arguments& args);
      static Persistent<Function> constructor;

      int m_misoPin;
      int m_ssPin;
      int m_clkPin;
};

#endif
