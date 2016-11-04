# !/bin/sh
# Auto pusher for server code to axis camera
# Author: Simon Gullstrand Warriv93
# Version: 0.2

make clean

create-package.sh mipsisa32r2el

eap-install.sh 192.168.2.101 pass install

eap-install.sh 192.168.2.101 pass start


echo "installed made clean server to axis camera"
echo "started server on axis camera"

echo "Tada! Woho!"

