docker run -v"$PWD:/sptrader" -ti -w/sptrader cmptech/auto_ubuntu1610_nodejs_sharessl sh -c "LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so /node-v7.7.4/bin/node test_sptrader.js"
