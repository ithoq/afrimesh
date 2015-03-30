# Read #

> http://weait.com/content/build-your-own-openstreetmap-server

> http://weait.com/content/make-your-first-map

> http://wiki.openstreetmap.org/wiki/Getting_Data

> http://trac.mapnik.org/wiki/UbuntuInstallation

> http://trac.mapnik.org/wiki/InstallationTroubleshooting

> http://wiki.openstreetmap.org/wiki/Mapnik

> http://wiki.openstreetmap.org/wiki/Contours

> http://code.google.com/p/mapnik-utils/wiki/Nik2Img

Run:
```
  sudo apt-get install imagemagick gdal-bin 
  cd ~/mapping-scripts
  svn co http://mapnik-utils.googlecode.com/svn/trunk/nik2img nik2img-svn
  cd nik2img-svn
  sudo python setup.py install
```

NOTE: World boundary data also available at: http://beta.letuffe.org/ressources/world_boundaries/compressed/processed_p.zip

NOTE: Shuttle Radar Tomography Mission data have moved to: http://dds.cr.usgs.gov/srtm/version2/SRTM3/

TODO: A script to grab the right SRTM data based on bounding box + continent


Get Data:
```
  wget "http://api.openstreetmap.org/api/0.6/map?bbox=28.0,-25.9,28.5,-25.6"
```

Assuming that all the tools are installed, data downloaded and contour data imported, the process for making a map boils down to:
```
  cd ~/mapnik
  source set-mapnik-env
  # import data, '-c' nukes existing data
  osm2pgsql -S ~/mapping-scripts/osm2pgsql-svn/default.style -c --slim -d gis ./csir.osm
  # big map
  nik2img.py ~/mapnik/osm.xml image.png --bbox 28.0 -25.9 28.5 -25.6 -d 5000 5000 -v 
  # tiles
  ./my_generate_tiles.py 
```