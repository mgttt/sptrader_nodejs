docker run -v"$PWD:/sptrader" -ti -w/sptrader \
cmptech/auto_ubuntu_essential_ssl \
sh -c "LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node-v7.8.0/bin/node test_sptrader.js"
