export TZ=Asia/Hong_Kong
echo run_api_server_in_docer.sh.date=`date`

#PY=`which python`
#if [ ! -n "$PY" ]; then
#	PY=`which python2.7`
#	if [ ! -n "$PY" ]; then
#		echo "not found python???"
#	else
#		echo "not found python, try link one.."
#		ln -s $PY /usr/bin/python
#	fi
#fi

#. /node_env.sh && npm install segfault-handler q && \

. /node_env.sh && \
	export PATH=/$NODE_VERSION/bin::$PATH && \
	node --expose_gc test_version.js $*

