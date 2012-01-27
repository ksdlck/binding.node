#include <iostream>
#include <string>
#include <v8.h>
#include <node.h>

/* Things get *real* cluttered otherwise */
using namespace std;
using namespace node;
using namespace v8;

/* Some handy macros for adding properties to your objects */
#define BINDING_SET_CONST_INT(target, name, val) ((target)->Set( \
      String::NewSymbol(#name), Integer::New(val), \
      static_cast<PropertyAttribute>(ReadOnly | DontDelete)))
#define BINDING_SET_CONST_STR(target, name, val) ((target)->Set( \
      String::NewSymbol(#name), String::New(str), \
      static_cast<PropertyAttribute>(ReadOnly | DontDelete)))
#define BINDING_SET_CONST_PROC(target, name, val) ((target)->Set( \
      String::NewSymbol(#name), FunctionTemplate::New(val)->GetFunction(), \
      static_cast<PropertyAttribute>(ReadOnly | DontDelete)))

#define BINDING_SET_INT(target, name, val) ((target)->Set( \
      String::NewSymbol(#name), Integer::New(val)))
#define BINDING_SET_STR(target, name, val) ((target)->Set( \
      String::NewSymbol(#name), String::New(str)))
#define BINDING_SET_PROC(target, name, val) ((target)->Set( \
      String::NewSymbol(#name), FunctionTemplate::New(val)->GetFunction()))

/* Some handy macros for adding properties to your prototypes */
#define BINDING_SET_PROTO_INT(tpl, name, val)
#define BINDING_SET_PROTO_STR(tpl, name, val)
#define BINDING_SET_PROTO_PROC(tpl, name, val) \
  do { \
    Local<Signature> __##tpl##_##name##_SIG = Signature::New(tpl); \
    Local<FunctionTemplate> __##tpl##_##name##_TEM = FunctionTemplate::New( \
        val, Handle<Value>(), __##tpl##_##name##_SIG); \
    tpl->PrototypeTemplate()->Set(String::NewSymbol(#name), \
        __##tpl##_##name##_TEM); \
  } while (0)

#define BINDING_SET_CONST_PROTO_INT(tpl, name, val)
#define BINDING_SET_CONST_PROTO_STR(tpl, name, val)
#define BINDING_SET_CONST_PROTO_PROC(tpl, name, val)

/* You don't really need this, but it doesn't hurt */
namespace binding
{

/* Constants */
enum constants
{
  ONE,
  TWO,
  THREE,
  FOUR
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
    BINDING_SET_PROTO_PROC(_pft, say, Say);

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
