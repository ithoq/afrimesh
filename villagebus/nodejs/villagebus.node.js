// node.js dependencies
var url  = require("url");
var http = require("http");
var path = require('path');
var querystring = require("querystring");
var paperboy = require("paperboy");
var multipart = require("multipart"); // npm install multipart

// other dependencies
var sessions = require("./session"); // https://github.com/Marak/session.js/blob/master/lib/session.js
var fail = require("./common").fail;

// load modules
var modules = {
  auth     : require("./modules/auth"),
  test     : require("./modules/test"),
  http     : require("./modules/http"),
  pubsub   : require("./modules/pubsub"),
  profiles : require("./modules/profiles"),
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

  // response helper
  response.fin = function(status, reply, headers, module) {
    status  = status  ? status         : 200;
    headers = headers ? headers        : {};
    module  = module  ? module + ": "  : "";
    console.log("fin " + module + status + " " + reply);
    if (request.redis.client) { // close any open redis session
      try {
        console.log("quit redis");
        request.redis.client.quit();
      } catch (e) {
        console.log("ERROR QUITTING REDIS: " + e);
      }
      request.redis.client = undefined;
    }
    headers["Content-Type"] = "text/plain"; // TODO pass content type e.g. "application/json";
    response.writeHead(status, headers);
    if (reply) response.write(JSON.stringify(reply));
    response.end();
  };


  // read post data 
  request.setEncoding("binary"); // otherwise multipart gets nuked
  request.addListener("data", function(chunk) {
    if (!request.data) request.data = "";
    request.data += chunk;
  }).on('end', function() {  // check if data is multipart, json or form encoded
    if (request.data && request.data.substring(0, 4) == "----") {
      var data = {};
      var parser = multipart.parser();
      var fields = {};
      var files  = {};
      var buffer = "";
      parser.headers = request.headers;
      parser.onpartend = function (part)  {  
        if (part.filename) files [part.filename] = buffer; 
        else               fields[part.name] = buffer; 
        buffer = "";
      };
      parser.ondata = function (chunk) { buffer += chunk; };
      parser.onend = function () { 
        request.data  = fields;
        request.files = files;
      };
      parser.write(request.data);
      parser.close();
      return;
    }
    try {
      request.data = JSON.parse(request.data);
      return;
    } catch (e) { }
    try {
      request.data = querystring.parse(request.data);
    } catch (e) { }
  });

  
  // handle request
  request.addListener("end", function() {

    console.log("\n----------------------------------------------------------------------");
    // parse request url
    var a = url.parse(unescape(request.url), true);
    request.path  = a.pathname.split(/\/+/g).slice(1);
    request.query = a.query;
    //request.data  = data;
    console.log("REST    : " + JSON.stringify({
      url   : request.url,
      verb  : request.method,
      path  : request.path,
      query : request.query,
      data  : request.data
    }));

    // get session
    request.session = sessions.lookupOrCreate(request, {
      lifetime : 3600
    });
    request.redis = function() {
      if (!request.redis.client) {
        request.redis.client = require("redis").createClient();
        request.redis.client.on("error", function(error) { 
          console.log("REDIS ERROR " + error); 
        });
      }
      return request.redis.client;
    }
    response.setHeader('Set-Cookie', request.session.getSetCookieHeaderValue());
    //console.log("Cookie  : " + request.session.getSetCookieHeaderValue());
    console.log("Session : " + JSON.stringify(request.session));

    // dispatch request
    var reply  = { error : "Unknown error for name: '" + name + "'", status : 200 };
    var name   = request.path.shift();
    var module = modules[name];
    if (!module) {                            // try to serve as a file
      return paperboy.deliver(config.webroot, request, response)
          .addHeader("Expires", 300)
          .addHeader("X-PaperRoute", "Node")
          .after(function(status) {
            //console.log(status + "\t" + request.url + "\t" + request.connection.remoteAddress);
          });
      
    } else if (module) {
      var namefn = request.path[0];
      if (module[namefn]) {               // if we have a namefn
        namefn = request.path.shift();
        if (module[namefn][request.method]) {  // invoke any REST verb attached to namefn
          reply = module[namefn][request.method](request, response);
        } else {                          // invoke namefn
          reply = module[namefn](request, response); 
        }
      } else if (module[request.method]) {     // invoke any REST verbs attached to module
        reply = module[request.method](request, response);
      } else if (module["evaluate"]) {    // invoke any evaluate function attached to module
        reply = module.evaluate(request, response);
      } else {
        return response.fin(404, "Could not resolve name '" + namefn + "' in module: " + name);
      }

    } else {
      return response.fin(420, "enhance your calm");
    }

    // reply is undefined if module wants to handle the response itself
    if (reply) {
      return response.fin(200, reply, {}, "villagebus"); // TODO use status from fail above
    } 
  });
}).listen(config.port);
console.log('VillageBus node running at http://127.0.0.1:8000/');

