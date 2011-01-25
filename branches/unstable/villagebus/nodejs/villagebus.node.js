
// node.js dependencies
var url  = require("url");
var http = require("http");
var path = require('path');
var querystring = require("querystring");
var paperboy = require("paperboy");

// other dependencies
var sessions = require("./session"); // https://github.com/Marak/session.js/blob/master/lib/session.js
var fail = require("./common").fail;

// load modules
var modules = {
  auth : require("./modules/auth"),
  test : require("./modules/test"),
  http : require("./modules/http")
};

// exit handler
process.addListener("SIGINT", function () {
  console.log("good bye\n");
  process.exit(0);
});

// configuration
config = {
  port    : 8000,
  webroot : path.join(path.dirname(__filename), 'webroot')
};
// console.log("__filename: " + __filename);

// fix response.writeHead
var _writeHead = http.ServerResponse.prototype.writeHead;
http.ServerResponse.prototype.setHeader = function (key, value) {
  this._additionalHeaders = this._additionalHeaders || {}; 
  this._additionalHeaders[key] = value;
};
http.ServerResponse.prototype.writeHead = function (status, headers) {
  var that = this;
  if (this._additionalHeaders) {
    Object.keys(this._additionalHeaders).forEach(function (k) {
      headers[k] = headers[k] || that._additionalHeaders[k];
    }); 
  }   
  _writeHead.call(this, status, headers);
};


// main server request handler
http.createServer(function (request, response) {

  var data = undefined;
  request.addListener("data", function(chunk) {
    if (!data) data = "";
    data += chunk;
  }).on('end', function() {  // check if data is json or form encoded
    try {
      data = JSON.parse(data);
      return;
    } catch (e) { }
    try {
      data = querystring.parse(data);
    } catch (e) { }
  });

  
  request.addListener("end", function() {

    console.log("\n----------------------------------------------------------------------");
    var a = url.parse(unescape(request.url), true);
    var rest = {
      verb  : request.method,
      path  : a.pathname.split(/\/+/g).slice(1),
      query : a.query,
      data  : data
    };
    console.log("REST : " + JSON.stringify(rest));

    // get session
    request.session = sessions.lookupOrCreate(request, {
      lifetime : 3600
    });
    response.setHeader('Set-Cookie', request.session.getSetCookieHeaderValue());
    console.log(request.session);


    // dispatch request
    var reply  = { error : "Unknown error for name: '" + name + "'", status : 200 };
    var name   = rest.path.shift();
    var module = modules[name];
    if (!module) {                            // try to serve as a file
      return paperboy.deliver(config.webroot, request, response)
              .addHeader("Expires", 300)
              .addHeader("X-PaperRoute", "Node")
              .after(function(status) {
                  //console.log(status + "\t" + request.url + "\t" + request.connection.remoteAddress);
                });

    } else if (module[rest.verb]) {   // try the REST verbs
      reply = module[rest.verb](request, response, rest);

    } else if (module["evaluate"]) {  // look for an evaluate function
      reply = module.evaluate(request, response, rest);

    } else if (module) {
      var namefn = rest.path.shift();
      if (module[namefn]) {
        reply = module[namefn](request, response, rest);
      } else {
        reply = fail("Could not resolve name '" + namefn + "' in module: " + name);
        reply.status = 404;
      }

    } else {
      reply.status = 420; // enhance your calm
    }

    // reply is undefined if module wants to handle the response itself
    if (reply) {
      response.writeHead(reply.status ? reply.status : 200, { 
        //'Content-Type': 'application/json'
        'Content-Type': 'text/plain' 
      });
      response.write(JSON.stringify(reply) + "\n");
      response.end();
    } 

  });

  
}).listen(config.port);
console.log('VillageBus node running at http://127.0.0.1:8000/');


function session(request, options) {
  
};
