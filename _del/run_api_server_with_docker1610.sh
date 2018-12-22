#e.g.
#sh run_api_server_with_docker.sh "-p4444:5555" -p=4444
#curl http://127.0.0.1:5555/ -d {m:SPAPI_GetDllVersion}

SPTRADERDIR=$(cd `dirname $0`; pwd)
cd $SPTRADERDIR
docker run -v"$PWD:/sptrader" -ti -w/sptrader $1 \
	cmptech/auto_ubuntu1610_nodejs_sharessl \
	sh run_api_server_in_docker.sh $*
