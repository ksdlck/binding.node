#include <iostream>
#include <string>
#include <v8.h>
#include <node.h>

/* Things get *real* cluttered otherwise */
using namespace std;
using namespace node;
using namespace v8;

/* Some handy macros for adding properties to your objects */
#define BINDING_SET(type, obj, name, val) \
  ((obj)->Set(String::NewSymbol(#name), type::New(val)))
#define BINDING_SET_NUM(obj, name, val) BINDING_SET(Number, (obj), name, (val))
#define BINDING_SET_INT(obj, name, val) BINDING_SET(Integer, (obj), name, (val))
#define BINDING_SET_STR(obj, name, val) BINDING_SET(String, (obj), name, (val))
#define BINDING_SET_PROC(obj, name, val) \
  ((obj)->Set(String::NewSymbol(#name), \
    FunctionTemplate::New(val)->GetFunction()))

#define BINDING_SET_CONST(type, obj, name, val) \
  ((obj)->Set(String::NewSymbol(#name), type::New(val), \
    static_cast<PropertyAttribute>(ReadOnly | DontDelete)))
#define BINDING_SET_CONST_NUM(obj, name, val) BINDING_SET_CONST(Number, (obj), name, (val))
#define BINDING_SET_CONST_INT(obj, name, val) BINDING_SET_CONST(Integer, (obj), name, (val))
#define BINDING_SET_CONST_STR(obj, name, val) BINDING_SET_CONST(String, (obj), name, (val))
#define BINDING_SET_CONST_PROC(obj, name, val)\
  ((obj)->Set(String::NewSymbol(#name), FunctionTemplate::New(val)->GetFunction(), \
    static_cast<PropertyAttribute>(ReadOnly | DontDelete)))

/* Some handy macros for adding properties to your prototypes */
#define BINDING_SET_PROTO(type, tpl, name, val) \
  ((tpl)->PrototypeTemplate()->Set(String::NewSymbol(#name), type::New(val)))
#define BINDING_SET_PROTO_NUM(tpl, name, val) BINDING_SET_PROTO(Number, (tpl), name, (val))
#define BINDING_SET_PROTO_INT(tpl, name, val) BINDING_SET_PROTO(Integer, (tpl), name, (val))
#define BINDING_SET_PROTO_STR(tpl, name, val) BINDING_SET_PROTO(String, (tpl), name, (val))
#define BINDING_SET_PROTO_PROC(tpl, name, val) \
  do { \
    Local<Signature> __##tpl##_##name##_SIG = Signature::New(tpl); \
    Local<FunctionTemplate> __##tpl##_##name##_TEM = FunctionTemplate::New( \
        val, Handle<Value>(), __##tpl##_##name##_SIG); \
    tpl->PrototypeTemplate()->Set(String::NewSymbol(#name), \
        __##tpl##_##name##_TEM); \
  } while (0)

#define BINDING_SET_PROTO_CONST(type, tpl, name, val) \
  ((tpl)->PrototypeTemplate()->Set(String::NewSymbol(#name), type::New(val), \
    static_cast<PropertyAttribute>(ReadOnly | DontDelete)))
#define BINDING_SET_PROTO_CONST_NUM(tpl, name, val) BINDING_SET_PROTO_CONST(Number, (tpl), name, (val))
#define BINDING_SET_PROTO_CONST_INT(tpl, name, val) BINDING_SET_PROTO_CONST(Integer, (tpl), name, (val))
#define BINDING_SET_PROTO_CONST_STR(tpl, name, val) BINDING_SET_PROTO_CONST(String, (tpl), name, (val))
#define BINDING_SET_PROTO_CONST_PROC(tpl, name, val) \
  do { \
    Local<Signature> __##tpl##_##name##_SIG = Signature::New(tpl); \
    Local<FunctionTemplate> __##tpl##_##name##_TEM = FunctionTemplate::New( \
        val, Handle<Value>(), __##tpl##_##name##_SIG); \
    tpl->PrototypeTemplate()->Set(String::NewSymbol(#name), \
        __##tpl##_##name##_TEM, static_cast<PropertyAttribute>(ReadOnly | DontDelete)); \
  } while (0)

/* Add a property to your instance template */
#define BINDING_SET_INST_PROP(tpl, name, getter, setter) \
  (tpl->InstanceTemplate()->SetAccessor(String::New(#name), (getter), (setter)))

/* You don't really need this, but it doesn't hurt */
namespace binding
{

/* Constants */
char* VERSION = "0.0.1";
enum NUMBERS
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
  static Persistent<FunctionTemplate> ctor;

  /* Initialize the Person class */
  static void Init(Handle<Object> target) {
    HandleScope scope;

    /* constructor */
    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    ctor = Persistent<FunctionTemplate>::New(t);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(String::NewSymbol("Person"));

    /* properties */
    BINDING_SET_PROTO_STR(ctor, laugh, "AHAHAHAHA");
    BINDING_SET_PROTO_INT(ctor, nine, 9);
    BINDING_SET_INST_PROP(ctor, name, GetName, SetName);

    /* methods */
    BINDING_SET_PROTO_PROC(ctor, say, Say);

    /* export */
    target->Set(String::NewSymbol("Person"), ctor->GetFunction());
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

  typedef struct _SayData {
    string name;
    string speech;
    Persistent<Function> cont;
  } SayData;

  /* Make the Person say something */
  static Handle<Value> Say(const Arguments& args) {
    HandleScope scope;
    Person* self = ObjectWrap::Unwrap<Person>(args.This());
    /* extract the speech and continuation */
    if (args.Length() < 2 || !args[1]->IsFunction())
      return ThrowException(Exception::TypeError(String::New("Need a string and a function")));
    String::Utf8Value speech(args[0]);
    Local<Function> cont = Local<Function>::Cast(args[1]);
    /* say something asynchronously */
    SayData* data = new SayData;
    /* We could also hold a reference to the Person instead of copying their
     * name, by doing self->Ref(), and self->Unref() in our continuation */
    data->name = string(self->name.c_str());
    data->speech = string(*speech);
    data->cont = Persistent<Function>::New(cont);
    /* I use uv_queue_work here rather than uv_write or calling into the node
     * stdlib, because this is more generally useful.  Libraries whose calls are
     * non-blocking already needn't concern themselves with libuv at all, and
     * libraries whose calls *are* blocking must either put them on the
     * threadpool or spawn their own thread to do the work, the latter being
     * outside the scope of this example. */
    uv_work_t* req = new uv_work_t;
    req->type = UV_WORK;
    req->loop = uv_default_loop(); /* TODO This should just be Loop(), but the compiler is whining */
    req->data = data;
    req->work_cb = SayWork;
    req->after_work_cb = SayAfterWork;
    uv_queue_work(req->loop, req, req->work_cb, req->after_work_cb);
    return Undefined();
  }

  static void SayWork(uv_work_t* req) {
    SayData* data = static_cast<SayData*>(req->data);
    cout << data->name << ": \"" << data->speech << "\"\n";
  }

  static void SayAfterWork(uv_work_t* req) {
    SayData* data = static_cast<SayData*>(req->data);
    /* do the rest inside a try/catch so asynchronous errors get thrown */
    TryCatch tc;
    data->cont->Call(Context::GetCurrent()->Global(), 0, NULL);
    data->cont.Dispose();
    delete data;
    if(tc.HasCaught()) FatalException(tc);
  }
}; /* END class Person */

/* Identity function */
Handle<Value> Id(const Arguments& args) {
  return args[0];
}

/* Initialize the binding */
static void Init(Handle<Object> target) {
  /* Initialize constants */
  BINDING_SET_CONST_INT(target, one, ONE);
  BINDING_SET_CONST_INT(target, two, TWO);
  BINDING_SET_CONST_INT(target, three, THREE);
  BINDING_SET_CONST_INT(target, four, FOUR);
  BINDING_SET_CONST_STR(target, version, VERSION);

  /* Initialize classes */
  Person::Init(target);

  /* Initialize procedures */
  BINDING_SET_PROC(target, id, Id);
}

} /* END namespace binding */

/* This symbol needs to exist in this scope for the library to load */
Persistent<FunctionTemplate> binding::Person::ctor;

/* Module entrypoint */
extern "C" {
  static void init(Handle<Object> target) {
    binding::Init(target);
  }

  /* NOTE the module name *must* match the filename */
  NODE_MODULE(binding, init);
}
