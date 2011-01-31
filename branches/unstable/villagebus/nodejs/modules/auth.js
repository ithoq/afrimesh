
var fail = require("../common").fail;
console.log("loaded module: auth"); // TODO - __filename


exports.session = {
  GET : function(request, response, rest) {
    return request.session.data;
  }
};


exports.logout = {
  GET : function(request, response, rest) {
    request.session.data.userid     = -1;
    request.session.data.username   = "guest";
    request.session.data.authorized = false;
    response.writeHead(302, { 
      'Location' : '/villagebus.tests.html'
    });
    response.end();
  }
};


// login -> POST /auth/register { username : <username>, sha1 : <sha1> } -> <userid>
// ccurl -X POST -d '{"username":"antoine@7degrees.co.za","sha1":"210b263cd3aed1604792c1449058e40d6ae3b86b"}' http://127.0.0.1:8000/auth/login
exports.login = {
  POST : function(request, response, rest) {
    if (!rest.data.username && !rest.data.sha1) {
      return response.fin(401, "invalid credentials: " + rest.data);   
    }
    var client = request.session.redis();
    client.get("username:" + rest.data.username + ":userid", function(error, userid) {
      if (error) return response.fin(500, error);
      if (!userid) return response.fin(401, "no such user: " + rest.data.username);
      client.get("userid:" + userid + ":sha1", function (error, sha1) {
        if (error) return response.fin(500, error);
        if (!sha1) return response.fin(401, "no hash for userid: " + userid);
        if (rest.data.sha1 != sha1) return response.fin(401, "invalid password");
        request.session.data.userid     = userid;
        request.session.data.username   = rest.data.username;
        request.session.data.authorized = true;
        response.fin(302, request.session.data, { // redirect
          'Location' : '/villagebus.tests.html'
        });
      });
    });
  }
};


// register -> POST /auth/register { username : <username>, sha1 : <sha1> } -> <userid>
// curl -X POST -d '{"username":"antoine@7degrees.co.za","210b263cd3aed1604792c1449058e40d6ae3b86b":"1234"}' http://127.0.0.1:8000/auth/register
exports.register = {
  POST : function(request, response, rest) {
    var client = request.session.redis();    
    // TODO check if user already exists
    return client.get("username:" + rest.data.username + ":userid", function(error, userid) {
      if (error) return response.fin(500, error);
      if (userid) return response.fin(409, "username already exists");
      create(rest.data.username, rest.data.sha1);
    });

    function create(username, sha1) {
      client.incr("next:userid", function(error, userid) {
        if (error) return response.fin(500, error);
        var profile = { userid : userid,
                        username : username };
        client.multi()
            .zadd("users:username", 0, username)
            .sadd("users:userid", userid)
            .mset([ "userid:"   + userid   + ":username", username,
                    "userid:"   + userid   + ":sha1",     sha1,
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


