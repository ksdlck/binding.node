#include <iostream>
#include <string>
#include <v8.h>
#include <node.h>

using namespace std;
using namespace node;
using namespace v8;

namespace binding
{

/* Constants */
enum constants
{
  ONE, TWO, THREE, FOUR
};

/* Person */
class Person: ObjectWrap
{
private:
  /* Person name */
  string name;

public:
  /* Person constructor template */
  static Persistent<FunctionTemplate> _pft;

  /* Initialize the Person class */
  static void Init(Handle<Object> target) {
    HandleScope scope;

    /* constructor */
    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    _pft = Persistent<FunctionTemplate>::New(t);
    _pft->InstanceTemplate()->SetInternalFieldCount(1);
    _pft->SetClassName(String::NewSymbol("Person"));

    /* properties */
    _pft->InstanceTemplate()->SetAccessor(String::New("name"), GetName, SetName);

    /* methods */
    NODE_SET_PROTOTYPE_METHOD(_pft, "say", Say);

    /* export */
    target->Set(String::NewSymbol("Person"), _pft->GetFunction());
  }

  Person() {
  }

  ~Person() {
  }

  /* Create a new Person */
  static Handle<Value> New(const Arguments& args) {
    HandleScope scope;
    Person* self = new Person();
    self->name = "Default";
    self->Wrap(args.This());
    return args.This();
  }

  /* Get the Person's name */
  static Handle<Value> GetName(Local<String> prop, const AccessorInfo& info) {
    Person* self = ObjectWrap::Unwrap<Person>(info.Holder());
    return String::New(self->name.c_str());
  }

  /* Set the Person's name */
  static void SetName(Local<String> prop, Local<Value> val, const AccessorInfo& info) {
    Person* self = ObjectWrap::Unwrap<Person>(info.Holder());
    self->name = *String::Utf8Value(val);
  }

  /* Make the Person say something */
  static Handle<Value> Say(const Arguments& args) {
    HandleScope scope;
    Person* self = ObjectWrap::Unwrap<Person>(args.This());
    String::Utf8Value speech(args[0]);
    cout << self->name << ": \"" << *speech << "\"\n";
    return Boolean::New(true);
  }
}; /* END class Person */

/* Identity function */
Handle<Value> Id(const Arguments& args) {
  return args[0];
}

/* Initialize the binding */
static void Init(Handle<Object> target) {
  /* Initialize constants */
  NODE_DEFINE_CONSTANT(target, ONE);
  NODE_DEFINE_CONSTANT(target, TWO);
  NODE_DEFINE_CONSTANT(target, THREE);
  NODE_DEFINE_CONSTANT(target, FOUR);

  /* Initialize classes */
  Person::Init(target);

  /* Initialize procedures */
  NODE_SET_METHOD(target, "id", Id);
}

} /* END namespace binding */

/* TODO This symbol needs to exist outside the */
Persistent<FunctionTemplate> binding::Person::_pft;

/* Module entrypoint */
extern "C" {
  static void init(Handle<Object> target) {
    binding::Init(target);
  }

  /* NOTE the module name *must* match the filename */
  NODE_MODULE(binding, init);
}
