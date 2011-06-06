
var fail = require("../common").fail;
console.log("loaded module: auth"); // TODO - __filename


exports.session = {
  GET : function(request, response) {
    return request.session.data;
  }
};


exports.logout = {
  GET : function(request, response) {
    request.session.data.userid     = -1;
    request.session.data.username   = "guest";
    request.session.data.authorized = false;
    response.writeHead(302, { 
      'Location' : '/villagebus.tests.html'
    });
    response.end();
  }
};


// login -> POST /auth/register { username : <username>, hash : <hash> } -> <userid>
// ccurl -X POST -d '{"username":"antoine@7degrees.co.za","hash":"210b263cd3aed1604792c1449058e40d6ae3b86b"}' http://127.0.0.1:8000/auth/login
exports.login = {
  POST : function(request, response) {
    if (!request.data.username && !request.data.hash) {
      return response.fin(401, "invalid credentials: " + request.data);   
    }
    var client = request.redis();
    client.get("username:" + request.data.username + ":userid", function(error, userid) {
      if (error) return response.fin(500, error);
      if (!userid) return response.fin(401, "no such user: " + request.data.username);
      client.get("userid:" + userid + ":hash", function (error, hash) {
        if (error) return response.fin(500, error);
        if (!hash) return response.fin(401, "no hash for userid: " + userid);
        if (request.data.hash != hash) return response.fin(401, "invalid password");
        request.session.data.userid     = userid;
        request.session.data.username   = request.data.username;
        request.session.data.authorized = true;
        response.fin(302, request.session.data, { // redirect
          'Location' : '/villagebus.tests.html'
        });
      });
    });
  }
};


// register -> POST /auth/register { username : <username>, hash : <hash> } -> <userid>
// curl -X POST -d '{"username":"antoine@7degrees.co.za","210b263cd3aed1604792c1449058e40d6ae3b86b":"1234"}' http://127.0.0.1:8000/auth/register
exports.register = {
  POST : function(request, response) {
    var client = request.redis();    
    // TODO check if user already exists
    return client.get("username:" + request.data.username + ":userid", function(error, userid) {
      if (error) return response.fin(500, error);
      if (userid) return response.fin(409, "username already exists");
      create(request.data.username, request.data.hash);
    });

    function create(username, hash) {
      client.incr("next:userid", function(error, userid) {
        if (error) return response.fin(500, error);
        var profile = { userid : userid,
                        username : username };
        client.multi()
            .zadd("usernames:username", 0, username)
            .zadd("usernames:userid", 0, userid)
            .mset([ "userid:"   + userid   + ":username", username,
                    "userid:"   + userid   + ":hash",     hash,
                    "userid:"   + userid   + ":profile",  JSON.stringify(profile),
                    "username:" + username + ":userid",   userid ])
            .exec(function(error, reply) {
              if (error) return response.fin(500, error);
              request.session.data.userid     = userid;
              request.session.data.username   = username;
              request.session.data.authorized = true;
              response.fin(302, userid, { 
                'Location' : '/villagebus.tests.html'
              });
            });
      });
    };
  }
};


