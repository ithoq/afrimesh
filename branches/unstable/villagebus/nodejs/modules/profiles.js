var redis = require("redis"); // npm install redis
var fail = require("../common").fail;

var pubsub = require("./pubsub");

console.log("loaded module: profile"); // TODO - __filename

// profiles -> GET /profiles?[username|userid=] -> <profiles>  
//             Only returns profiles with matching prefix if username specified
// curl -X GET http://127.0.0.1:8000/profiles
exports.GET = function(request, response) { // TODO - move rest.* to request object
  var client = request.session.redis();
  if (request.query.userid) {                                         // userid 
    return client.multi()
        .get("userid:" + request.query.userid + ":profile")
        .smembers("userid:" + request.query.userid + ":followers")
        .smembers("userid:" + request.query.userid + ":following").exec(function(error, reply) {
          if (error) return response.fin(500, error);
          if (!reply[0]) return response.fin(404, "no profile for userid:" + request.query.userid);
          var profile = JSON.parse(reply[0]);
          profile.followers = reply[1];
          profile.following = reply[2];
          return response.fin(200, profile);
        });
  }
  return client.zrange("usernames:userid", 0, -1, function(error, userids) {// everyone
    if (error) return response.fin(500, error);
    var multi = client.multi();
    for (var i in userids) {
      multi.get("userid:" + userids[i] + ":profile")
          .smembers("userid:" + userids[i] + ":followers")
          .smembers("userid:" + userids[i] + ":following");
    }
    multi.exec(function(error, reply) {
      if (error) return response.fin(500, error);
      var profiles = [];
      for (var i = 0; i < reply.length;) {
        var profile = JSON.parse(reply[i++]);
        profile.followers = reply[i++];
        profile.following = reply[i++];
        profiles.push(profile);
      }
      return response.fin(200, profiles);
    });
  });
};



// following -> POST /profiles/following { username|userid : <username|userid> }
//              follow username or userid
// following -> DELETE /profiles/following?[username|userid=]
//              unfollow username or userid
// ccurl -X POST -d '{"username":"antoine@7degrees.co.za"}' "http://127.0.0.1:8000/profiles/following"
// ccurl -X DELETE "http://127.0.0.1:8000/profiles/following?username=antoine@7degrees.co.za"
exports.following = {
  POST : function(request, response) {
    if (!request.session.data.authorized) return response.fin(401, "not logged in");
    if (!request.data.username && !request.data.userid) return response.fin(400, "specify username or userid");
    var client = request.session.redis();
    if (request.data.username) { // if username specified, need to look up uid
      return client.get("username:" + request.data.username + ":userid", function(error, userid) {
        if (error) return response.fin(500, error);
        if (!userid) return response.fin(404, "no username:" + request.data.username);
        follow(userid);
      });
    }
    follow(request.data.userid);
    function follow(userid) {
      client.multi()
          .get ("userid:" + userid + ":username")
          .sadd("userid:" + request.session.data.userid + ":following", userid)
          .sadd("userid:" + userid + ":followers", request.session.data.userid)
          .exec(function(error, reply) {
            if (error) return response.fin(500, error);
            if (!reply[0]) return response.fin(404, "no username found for userid:" + userid);
            request.data.channel = reply[0];
            return pubsub.subscribers.POST(request, response);
          });
    }
  },

  DELETE : function(request, response) {
    if (!request.session.data.authorized)           return response.fin(401, "not logged in");
    if (!request.query.username && !request.query.userid) return response.fin(400, "specify username or userid");
    var client = request.session.redis();
    if (request.query.username) { // if username specified, need to look up uid
      return client.get("username:" + request.query.username + ":userid", function(error, userid) {
        if (error) return response.fin(500, error);
        unfollow(userid);
      });
    }
    unfollow(request.query.userid);
    function unfollow(userid) {
      client.multi()
          .get ("userid:" + userid + ":channelid")
          .srem("userid:" + request.session.data.userid + ":following", userid)
          .srem("userid:" + userid + ":followers", request.session.data.userid)
          .exec(function(error, reply) {
            if (error) return response.fin(500, error);
            if (!reply[0]) return response.fin(500, "no channelid found for userid:" + userid);
            request.query.channelid = reply[0];
            return pubsub.subscribers.DELETE(request, response);
          });
    }
  }
};

