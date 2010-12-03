

all : villagebus

#sources=villagebus.lua urlcode.lua provision.lua http.lua 
sources=urlcode.lua villagebus.lua 
modules=modules/provision.lua modules/http.lua 

villagebus : $(sources)
	luac -o villagebus.luac $(sources)
	#luac -o villagebus.modules.luac $(modules)
	luac -o modules/modules.luac $(modules)

# tests

GET=PATH_INFO="/http/192.168.20.100:80/index.html" \
    REQUEST_METHOD="GET" \
    QUERY_STRING="foo=bar&plink=plonk"	

# PATH_INFO="/http/example.com/index.html" REQUEST_METHOD="GET" ./villagebus.lua

get :
	$(GET) ./villagebus.lua
	#$(GET) lua ./villagebus.luac


POST=PATH_INFO="/http/192.168.20.100:80/cgi-bin/villagebus.lua/provision/router/00:11:22:33:44:55" \
     REQUEST_METHOD="POST" \
     QUERY_STRING="foo=bar&plink=plonk" \
     CONTENT_LENGTH=1

post :
	echo '{ "a" : "1", "b" : "2", "c" : "3" }' | $(POST) ./villagebus.lua

clean :
	rm -f *~ *.luac 
	rm -f modules/*~ modules/*.luac
