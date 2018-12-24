export TZ=Asia/Hong_Kong
echo run_api_server_in_docer.sh.date=`date`

#ulimit -c unlimited && ulimit -a && . /node_env.sh && \
#npm install segfault-handler q && \
#export PATH=/$NODE_VERSION/bin:$PATH && \
ulimit -c unlimited && ulimit -a
LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so \
node --expose_gc test_version.js $*

#node test_ver.js $*

# self global.gc():
#node --expose_gc test_ver.js

##!/bin/sh
#
#. /node_env.sh
#
#export PATH=/$NODE_VERSION/bin::$PATH
#
#echo `which node` `node -v`
#echo `which node-gyp`
#
#cd $(dirname "$0")
#
#pwd
#
##TODO
##--expose_gc
#
#LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so \
#	node --expose_gc test_ver.js
