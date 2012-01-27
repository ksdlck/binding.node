#include <iostream>
#include <string>
#include <v8.h>
#include <node.h>

using namespace std;
using namespace node;
using namespace v8;

class Binding: ObjectWrap
{
public:
  static Persistent<FunctionTemplate> _pft;

  static void Init(Handle<Object> target) {
    HandleScope scope;

    /* constructor */
    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    _pft = Persistent<FunctionTemplate>::New(t);
    _pft->InstanceTemplate()->SetInternalFieldCount(1);
    _pft->SetClassName(String::NewSymbol("Binding"));

    /* properties */
    _pft->InstanceTemplate()->SetAccessor(String::New("name"), GetName, SetName);

    /* methods */
    NODE_SET_PROTOTYPE_METHOD(_pft, "say", Say);

    /* export */
    target->Set(String::NewSymbol("Binding"), _pft->GetFunction());
  }

  Binding() {
  }

  ~Binding() {
  }

  string name;

  static Handle<Value> New(const Arguments& args) {
    HandleScope scope;
    Binding* self = new Binding();
    self->name = "Default";
    self->Wrap(args.This());
    return args.This();
  }

  static Handle<Value> GetName(Local<String> prop, const AccessorInfo& info) {
    Binding* self = ObjectWrap::Unwrap<Binding>(info.Holder());
    return String::New(self->name.c_str());
  }

  static void SetName(Local<String> prop, Local<Value> val, const AccessorInfo& info) {
    Binding* self = ObjectWrap::Unwrap<Binding>(info.Holder());
    self->name = *String::Utf8Value(val);
  }

  static Handle<Value> Say(const Arguments& args) {
    HandleScope scope;
    Binding* self = ObjectWrap::Unwrap<Binding>(args.This());
    String::Utf8Value speech(args[0]);
    cout << self->name << ": \"" << *speech << "\"\n";
    return Boolean::New(true);
  }
};

Persistent<FunctionTemplate> Binding::_pft;

extern "C" {
  static void init(Handle<Object> target) {
    Binding::Init(target);
  }

  NODE_MODULE(binding, init);
}
