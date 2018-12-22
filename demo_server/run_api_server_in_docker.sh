export TZ=Asia/Hong_Kong
echo run_api_server_in_docer.sh.date=`date`

ulimit -c unlimited && ulimit -a && . /node_env.sh && \
export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so \
node --expose_gc sptrader_api_server.js $*

#TODO
#-approot=/sptrader/ -jobs="['Health','HSI','Strategy']"
#node -e "require('nodenodenode').daemon({approot:process.cwd(),http_port:4321})" /jobs="['HSI','Strategy']" $*
