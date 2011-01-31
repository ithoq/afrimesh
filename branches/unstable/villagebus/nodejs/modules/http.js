
var http = require("http");
var fail = require("../common").fail;

console.log("loaded module: http");

exports.evaluate = function(request, response) {

  var hostname = request.path.shift();
  if (!hostname) return fail("no hostname in request", "http");
  var port = 80; // TODO - parse port if present
  var path = "/" + request.path.join("/");
  console.log("hostname : " + hostname);
  console.log("port     : " + port);
  console.log("path     : " + path);

  var httpclient = http.createClient(port, hostname);
  var httprequest = httpclient.request(request.method, path, { host : hostname });
  httprequest.end();
  httprequest.on('response', function (httpresponse) {
    console.log('STATUS  : ' + httpresponse.statusCode);
    console.log('HEADERS : ' + JSON.stringify(httpresponse.headers));
    response.writeHead(httpresponse.statusCode, httpresponse.headers);
    httpresponse.setEncoding('utf8');
    httpresponse.on('data', function (chunk) {
      console.log('BODY: ' + chunk);
      response.write(chunk);
      response.end();
    });
  });
  
  return undefined;
};
