var util = require("util");
var binding = require("./build/Release/binding");

/* test constants */
console.log("======================================================================================");
console.log("Constants");
console.log("--------------------------------------------------------------------------------------");
console.log("ONE = " + binding.one);
console.log("TWO = " + binding.two);
console.log("THREE = " + binding.three);
console.log("FOUR = " + binding.four);
console.log("--------------------------------------------------------------------------------------");
console.log();

/* test procedures */
console.log("======================================================================================");
console.log("Procedures");
console.log("--------------------------------------------------------------------------------------");
var three = 3;
var george = "George"
var author = {
  name: "Karel",
  email: "k@ksdlck.com"
};
console.log(three + " = " + binding.id(three));
console.log(george + " = " + binding.id(george));
console.log(util.inspect(author) + " = " + util.inspect(binding.id(author)));
console.log
console.log("--------------------------------------------------------------------------------------");
console.log();

/* test classes */
console.log("======================================================================================");
console.log("Classes");
console.log("--------------------------------------------------------------------------------------");
var b = new binding.Person();
console.log("By default, a Person's name is \"" + b.name + "\".");
b.name = "Bob";
console.log("But you can rename a Person to anything you like, for example, \"" + b.name + "\".");
console.log("Why not introduce yourself, " + b.name + "?");
b.say("Hi, I'm " + b.name);
console.log("--------------------------------------------------------------------------------------");
console.log();

/* test prototype properties */
console.log("======================================================================================");
console.log("Classes");
console.log("--------------------------------------------------------------------------------------");
console.log(b.name + "'s laughter sounds like \"" + b.laugh + "\"!");
console.log("The number nine is " + b.nine);
console.log("--------------------------------------------------------------------------------------");
