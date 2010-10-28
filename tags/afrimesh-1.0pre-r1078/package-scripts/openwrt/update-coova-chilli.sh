# json portal
wget http://coova.org/uam/
# javascript to drive it
wget http://coova.org/js/chilli.js
# fix js address 
sed -i 's/coova.org\/js\/chilli.js/\/portal\/chilli.js/g' index.html
# move it to destination
# mv chilli.js files/www/portal
# mv index.html files/www/portal