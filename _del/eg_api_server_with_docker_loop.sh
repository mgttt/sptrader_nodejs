port=$1

#if [ "$port" == "" ] # bash
if [ "$port" = "" ] #sh only know = instead of ==
then
	echo "port is needed!"
else
	echo "port=$port"
	#rm running$port.lck
	#sleep 2
	echo `date` > running$port.lck
fi

while [ -f $PWD/running$port.lck ]
do
	docker ps
	docker kill $(docker ps |grep $port | awk '{print $1}')

	#Remove all stopped containers.
	docker rm $(docker ps -a -q)

	#Remove all untagged images
	docker rmi -f $(docker images -a | grep "^<none>" | awk "{print \$3}")

	docker ps

	#sleep 1

	echo curl http://127.0.0.1:$port/ -d '{"m":"SPAPI_GetDllVersion"}'
	sh run_demo_api_server_with_docker.sh "-p$port:4444 -v$PWD:/sptrader_api_server" -server_id=$port -server_port=4444 -app=./sptrader_api_server_demo_logic.js 2>&1
#| node handle_stdin_log.js $port.log
done
echo "exit $port for not found running$port.lck"
