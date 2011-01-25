
var http  = require("http");
var redis = require("redis");
var fail  = require("../common").fail;

console.log("loaded module: pubsub");


/*

  Two kinds:

    -> realtime : as they come in you get it and it is removed from the queue
                  Q: anything coming in when you are not there is lost/buffered ?

    -> historic : first request gives you everything since the last time 
                  as they come in you get it and it is removed from the queue and appended to the historic queue
                  anything coming in when you are not there is buffered
                  optional: you can request any past message within a date/numeric range

 */


// poll -> GET /pubsub/<channel> -> <messages>
exports.GET = function(request, response, rest) {
  
};

// publish -> POST /pubsub/<channel>/<message> -> <channel>
exports.POST = function(request, response, rest) {
  
};

// subscribe -> PUT /pubsub/<channel>/<uid> -> 0
exports.PUT = function(request, response, rest) {
  
};

