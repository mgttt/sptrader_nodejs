#!/bin/sh

# suggest to build at linux directly.  (build in docker is not yet completed, because need some time to prepare a image with python inside...)

echo NODE_VERSION=$NODE_VERSION

#export NODE_PATH=$HOME/$NODE_VERSION/lib/node_modules
export PATH=$HOME/$NODE_VERSION/bin:$HOME/$NODE_VERSION/node_modules/.bin:$PATH
#echo PATH=$PATH
echo node:
echo `which node` `node -v`
echo node-gyp:
echo `which node-gyp`

cd $(dirname "$0")
pwd

rm -Rf build
mkdir ../tmp/
node-gyp configure
cp libapiwrapper.so build/

#node-gyp build 
sh -c "node-gyp build 2>&1" > build/log_build.log
cat build/log_build.log |grep -2 error

ls -al build/Release/SpTrader.node
NODE_MODULE_VER=`node -pe "var os=require('os');os.arch()+'-'+os.platform()+'-'+process.versions.modules"`
BUILD_AND_RENAME=SpTrader.$NODE_MODULE_VER.node
cp build/Release/SpTrader.node $BUILD_AND_RENAME
ls -al $BUILD_AND_RENAME
