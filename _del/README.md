# sptrader_nodejs

sptrader nodejs wrapper

## sptrader

http://www.sharppoint.com.hk/algo.php?lang=1&mod=api

# dependence

node with shared ssl is need to run the wrapper.  or using docker mode :)

## install and compile node with shared ssl

* node is default with openssl which openssl is conflict with the libapiwrapper shipped so need to compile it manually..


```
export NODE_VERSION=node-v7.7.4
cd ~

#wget https://nodejs.org/dist/latest/$NODE_VERSION.tar.gz
wget http://npm.taobao.org/mirrors/node/latest/$NODE_VERSION.tar.gz

tar xzvf $NODE_VERSION.tar.gz
cd $NODE_VERSION

#./configure --prefix=/$HOME/$NODE_VERSION --shared-openssl
./configure --prefix=. --shared-openssl

#compile with 8 cpu
make -j8

#make install

# need libssl (as shared lib to run libapiwrapper)
apt install libssl
```

## ~.bashrc

```
#export NODE_VERSION=node-v6.10.1
export NODE_VERSION=node-v7.7.4
export NODE_PATH=~/$NODE_VERSION/lib/node_modules
alias npm=~/$NODE_VERSION/bin/npm
export PATH=~/$NODE_VERSION/bin:$PATH
```

## CPU

```
node -e "console.log(require('os').cpus().length);"
```

## VERSIONS

```
node -e "console.log(process.versions);"
```


## buildin Libs

```
node -pe "require('repl')._builtinLibs"
```


## loaded modules

```
node -pe "Object.keys(require.cache);"
```
