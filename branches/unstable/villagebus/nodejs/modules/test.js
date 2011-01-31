
var redis = require("redis"); // npm install redis
var fail = require("../common").fail;

console.log("loaded module: test");

exports.farmer = function(request, response) {
  if (!request.session.data.authorized) {
    return fail("You are not logged in", "test", 401);
  }

  return { song : "I am a teapot, this is my spout" }; 
};

