
```
DAY 9 : CONTINUE WORK   !!!! 13:00 -> Staff meeting in Cafetaria !!!!

  JSON Overview: http://ennuidesign.com/blog/JSON:+What+It+Is,+How+It+Works,+and+How+to+Use+It/

  * Cynthia     ->  Script to generate a locally hosted map server
    1) Figure out how to pull the .osm data for a given bounding box with co-ordinates specified 
    2) Figure out how to do a Mapnik render of .png tiles from the .osm data 
       . Install Mapnik
       . Write a script to render the data retrieved above
       . Read: http://wiki.openstreetmap.org/wiki/Rendering
               http://wiki.openstreetmap.org/wiki/Mapnik
               http://wiki.openstreetmap.org/wiki/Pyrender

  * Jonathan    -> Vis Server + Integrate pmacct & NAGios with Dashboard
    1) See if there are JSON plugins for NAGios
    2) Check out pmacct source code and look at how you would modify it to output JSON
    3) Identify interesting data from pmacct/NAGios to display on dashboard map
    4) Resubmit vis server patch after testing&review

  * Edwe         ->  memcachedb for freeradius, maybe even cache dashboard map if there's time
    1) Compile and test the various functions in rlm_memcachedb
       /etc/init.d/freeradius stop
       freeradius -X
       radtest <username> <password> <radius_server> 10 [secret]

  * Nkosinathi  ->  Google Earth Export for Dashboard map
    1) install google earth
    2) read up about the google earth file format
    3) create link on the network map
    4) create an eventhandler for the link
    5) write a function that outputs some html to the browser when clicked
    6) extend function to output some data in google earth file format 
    7) extend function to output router information in google earth file format    

  * David        ->  Get up to speed
    1) Add node neighbors to network map popup -> Issue#18
    2) Make layout prettier and list neighbours in columns






------------------------------------------------------------------------------

DAY 10

------------------------------------------------------------------------------

  * See: http://www.meraka.org.za/afrimeshcodesprint.htm
  * 1. Generate a locally hosted map server
  * 2. JSON out for PMACCT/NAGios etc.
  * 3. Traffic control scripts
  * 4. Security evaluation
  * 5. 10 common billing database queries
  * 6. memcachedb backend for freeradius
  * 7. Google Earth import/export for dashboard map view
  * 8. customize coova-chilli page for routers
  * 9. Firmware Image Server

     -> vis server bugs.
    1) figure out why vis server is eating the whole cpu (!)
      . review patch w/ antoine
      . post patch
    2) figure out why vis server has 5/6 dupe entries for each route
      . review patch w/ antoine
      . post patch
   











------------------------------------------------------------------------------

DAY 1 - SETUP & GOAL
  . Introductions
  . Consent & Assessment forms
  . The re-arranging of the furniture
  . Tea/Coffee/Email
  . 13:00 -> START
  . gmail addresses -> antoinevangelderjnr@gmail.com  
  . Afrimesh mailing list -> http://lists.meraka.csir.co.za/mailman/listinfo/afrimesh
    To post to the list -> afrimesh@lists.csir.co.za
  . twitter -> http://twitter.com/louiscoetzee 
  . Wiki -> http://code.google.com/p/afrimesh/wiki/CodeSprint2009
  . Sammy's use case
  . Mesh networking, OLSR, MP
  . Bringing up a mesh with batmand

--

DAY 2 Morning : TOOLCHAIN & CODING
  . short history lesson
  . the free software development cycle
  . Project Websites: http://code.google.com/p/afrimesh
                      https://www.open-mesh.net/
  . source control: cvs/svn/git/hg 
    . check out batmand, compile, run
    . batman vis server
    . revisions
  . editors

  12:30 -> 13:30 : LUNCH


DAY 2 Afternoon 
  . bring up Afrimesh dev environment

--

DAY 3 Morning : FIRST TASKS
  . Debugging -> firebug
  . Move the mesh to Pretoria: /etc/config/afrimesh
    . Longitude   28.279280
    . Latitude   -25.755928
  . submitting patches
  . commit access
  . committing code
  . svn status
  . svn diff 
  . svn commit
  . svn update
  . svn revert
  . Ed&Nkosinathi -> Fix villagebus.batman() bug on client-side
  . Jonathan -> Fix vis server bug
  . Cynthia -> Improve map for CSIR
  . Tool: http://svn.collab.net/repos/svn/trunk/contrib/client-side/wcgrep

DAY 3 Afternoon : FIRST TASKS
  . Ed&Nkosinathi -> Set color of vis server text box dependent on whether it works
  . Cynthia -> start reading up on Python for the map scripting 
  . Ed -> Look at missing default settings in afrimesh.settings.js
  . Nkosinathi -> Reading up on Javascript

--

DAY 4 : COMMUNICATING IN THE OPEN SOURCE COMMUNITY
  . Show relationships between tasks & communications
  . Twitter
  . IRC 
  . Pastebin
  . Nkosinathi -> Stop enter from submitting form
                  Commit settings changes when you press enter
    - javascript event handlers:
    - jquery docs: Events->Event Handling->Bind

      $("#myForm").bind("keypress", function(event) {
          if (event.keyCode == 13) { 
            // save the field
          }
        });

  . Cynthia -> as yesterday
  . Ed -> as yesterday, set village-bus-radius from uci config
  . Nkosinathi -> Do frontend for radius config
  . Jonathan -> as yesterday, submit an upstream patch

--

DAY 5 : INDUCTION, WORK, CHAOS
  . Induction
  . Nkosinathi, Ed - finishing up configuration feature
  . David - B.A.T.M.A.N. intro by Elektra
  . Jonathan - vis server
  . Cynthia on leave.

--

DAY 6 & 7 : WEEKEND
  * Edwe, Jonathan, David hacked some
  * Antoine stabilized the network, some routers, freeradius, coova, etc.

--

DAY 8 : BEGIN WORK ON SPRINT TASKS
  
  * Cynthia     ->  Script to generate a locally hosted map server
    1) Figure out how to pull the .osm data for a given bounding box with co-ordinates specified 
         as:  left-longitude, bottom-latitude, right-longitude, top-latitude
         or:  center-longitude, center-latitude, extent-degrees
         or:  center-longitude, center-latitude, extent-kilometres
       . Write a script to automate the retrieval
       . Read:  http://wiki.openstreetmap.org/wiki/Getting_Data
    2) Figure out how to do a Mapnik render of .png tiles from the .osm data 
       . Write a script to render the data retrieved above
       . Read: http://wiki.openstreetmap.org/wiki/Rendering
               http://wiki.openstreetmap.org/wiki/Mapnik
               http://wiki.openstreetmap.org/wiki/Pyrender
    3) Figure out how to serve rendered .png tiles from the dashboard server
    4) Figure out how to do a CycleMap render
       . Read: http://www.opencyclemap.org/

  * Jonathan    -> Vis Server + Integrate pmacct & NAGios with Dashboard
    1) See if there are JSON plugins for NAGios
    2) Check out pmacct source code and look at how you would modify it to output JSON
    3) Identify interesting data from pmacct/NAGios to display on dashboard map
    4) Resubmit vis server patch after testing&review

  * Edwe         ->  memcachedb for freeradius, maybe even cache dashboard map if there's time
    1) Research memcachedb
    2) Check out memcachedb trunk
    3) Install dependencies: libmemcached, libevent, BerkeleyDB.47
    4) configure/make/make install memcachedb 
    5) Research writing a freeradius authentication module

  * Nkosinathi  ->  Google Earth Export for Dashboard map
    1) install google earth
    2) read up about the google earth file format
    3) create link on the network map
    4) create an eventhandler for the link
    5) write a function that outputs some html to the browser when clicked
    6) extend function to output some data in google earth file format 
    7) extend function to output router information in google earth file format    

  * David        ->  Get up to speed
    1) Add node neighbors to network map popup -> Issue#18
```