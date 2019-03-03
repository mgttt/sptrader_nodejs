#!/bin/sh

. /node_env.sh

#ln -s /opt/local/bin/python2.7 /usr/bin/python

PY=`which python`
if [ ! -n "$PY" ]; then
	echo "not found python, try link one.."
	ln -s `which python2.7` /usr/bin/python
fi

export PATH=/$NODE_VERSION/bin::$PATH

echo `which node` `node -v`
echo `which node-gyp`

cd $(dirname "$0")
pwd

rm -Rf build
#mkdir ../tmp/

mkdir -p build/
cp libapiwrapper.so build/

NODE_MODULE_VER=`node -pe "var os=require('os');os.arch()+'-'+os.platform()+'-'+process.versions.modules"`
BUILT_RENAME=SpTrader.$NODE_MODULE_VER.node

#sh -c "node-gyp build 2>&1" > build/log_build.log
#sh -c "node-gyp build --debug 2>&1" > build/log_build.log
#sh -c "node-gyp build --debug 2>&1" > log_build.log
#node-gyp --debug configure rebuild

sh -c "node-gyp --debug configure build 2>&1" > log_build.log
cat log_build.log |grep -2 error
cp build/Debug/SpTrader.node $BUILT_RENAME

#node-gyp configure --debug
#node-gyp build --debug

sh -c "node-gyp configure build 2>&1" > log_build.log
cat log_build.log |grep -2 error
cp build/Release/SpTrader.node $BUILT_RENAME.live

#rm -Rf build/

sleep 1

find build/
ls -al $BUILT_RENAME

npm install segfault-handler

#sh test_ver_inner.sh
