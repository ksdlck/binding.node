var binding = require("./build/Release/binding")

var b = new binding.Binding();
b.name = "Bob";
b.say("Hi, I'm " + b.name);
