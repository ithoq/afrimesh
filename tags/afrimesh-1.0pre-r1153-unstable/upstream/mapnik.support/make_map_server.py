from optparse import OptionParser
parser = OptionParser()

parser.set_defaults(download=True)
parser.add_option("--bbox", type="float", nargs=4, dest="point")
parser.add_option("--zoom", type="int", nargs=2, dest="zoom")
parser.add_option("--resolution", type="int", nargs=2, dest="size")
parser.add_option("--download", action="store_false", dest="download")
(options, args) = parser.parse_args()

#Get co-ordinate from the user

print "Here are your co-ordinates", options.point
print "The map size is",options.size[0]," by ",options.size[1]
min_long = options.point[0]
min_lat = options.point[1]
max_long = options.point[2]
max_lat = options.point[3]
wide = options.size[0]
height = options.size[1]
first_side = options.zoom[0]
second_side = options.zoom[1]
#This program gets or retrieve data from the internet and from some files 
import urllib  #using python library,open the file       
           
if options.download != True:
       #Downloads files from the internet       
	url = "http://api.openstreetmap.org/api/0.6/map?bbox="
	url += str(min_long) + ","
	url += str(min_lat) + ","
	url += str(max_long) + ","
	url += str(max_lat)
	print("Downloading: " + url)
	urllib.urlretrieve(url, "mapdata.osm")  #save data to 'mapdata.osm' in the python
	print "Yes!! You are done. Map is comming up soon"
else:
	print "No download"


import commands
osm2pgsql = commands.getoutput('osm2pgsql -S ~/mapping-scripts/osm2pgsql-svn/default.style --slim -d gis ./mapdata.osm')
#print osm2pgsql

command1 = 'nik2img.py ~/mapnik/osm.xml ~/image.png --bbox '
command1 += str(min_long) + " "
command1 += str(min_lat) + " "
command1 += str(max_long) + " "
command1 += str(max_lat) + " " 
command1 += '-d' + " "
command1 += str(wide) + " "
command1 += str(height) + " "
command1 += '-v'

print "running command: " + command1
commands.getoutput(command1)

command2 = './my_generate_tiles.py --bbox '
command2 += str(min_long) + " "
command2 += str(min_lat) + " "
command2 += str(max_long) + " "
command2 += str(max_lat) + " "
command2 += '--zoom' + " "
command2 += str(first_side) + " "
command2 += str(second_side)

#render_tiles(bbox, mapfile, tile_dir, 0, 1, "CSIR")
print "running command: " + command2
print commands.getoutput(command2)

#commands.getoutput('eog ~/image.png')
