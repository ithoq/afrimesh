
GET=PATH_INFO="/http/192.168.20.100:80/index.html" \
    REQUEST_METHOD="GET" \
    QUERY_STRING="foo=bar&plink=plonk"	

get :
	$(GET) ./villagebus.lua


POST=PATH_INFO="/http/192.168.20.100:80/cgi-bin/villagebus.lua/provision/router/00:11:22:33:44:55" \
     REQUEST_METHOD="POST" \
     QUERY_STRING="foo=bar&plink=plonk" \
     CONTENT_LENGTH=1

post :
	echo '{ "a" : "1", "b" : "2", "c" : "3" }' | $(POST) ./villagebus.lua