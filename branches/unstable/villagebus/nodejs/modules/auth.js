
var redis = require("redis"); // npm install redis
var fail = require("../common").fail;

console.log("loaded module: auth"); // TODO - __filename


exports.session = {
  GET : function(request, response, rest) {
    return request.session.data;
  }
};


exports.logout = {
  GET : function(request, response, rest) {
    request.session.data.authorized = false;
    request.session.data.username = "guest";
    // else redirect to login page
    console.log("redirecting...");
    response.writeHeader(302, { 
      'Location' : '/villagebus.tests.html'
    });
    response.end();

    return undefined;
  }
};


exports.login = {
  POST : function(request, response, rest) {
    // look up username & check hash
    if (!rest.data.username && !rest.data.sha1) {
      return fail("Invalid login: " + rest.path, "auth", 401);   
    }
    
    // check user in DB & return 401 if invalid
    request.session.data.authorized = true;
    request.session.data.username = rest.data.username;
    
    // else redirect to login page
    console.log("redirecting...");
    response.writeHeader(302, { 
      'Location' : '/villagebus.tests.html'
    });
    response.end();
    
    return undefined; 
  }
};


