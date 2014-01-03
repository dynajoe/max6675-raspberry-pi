#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "max6675.h"

using namespace v8;

Handle<Value> CreateObject(const Arguments& args) {
   HandleScope scope;
   return scope.Close(Max6675::NewInstance(args));
}

void InitAll(Handle<Object> exports, Handle<Object> module) {
   Max6675::Init();

   module->Set(String::NewSymbol("exports"),
      FunctionTemplate::New(CreateObject)->GetFunction());
}

NODE_MODULE(Max6675, InitAll)