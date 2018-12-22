#e.g.
#sh run_api_server_with_docker.sh "-p4444:5555" -p=4444
#curl http://127.0.0.1:5555/ -d {m:SPAPI_GetDllVersion}

cd `dirname $0`
pwd

#docker run -v"$PWD:/sptrader" -ti -w/sptrader $1 \
#	cmptech/auto_ubuntu1610_nodejs_sharessl \
#	sh run_api_server_in_docker.sh $*

export TZ=Asia/Hong_Kong
echo run_api_server_with_docker.sh.date=`date`

docker run -v"$PWD:/sptrader" -i -w/sptrader $1 \
	cmptech/auto_ubuntuessential_nodejs_sharessl \
	sh run_demo_api_server_in_docker.sh $*

