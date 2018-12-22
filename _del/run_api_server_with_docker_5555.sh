export TZ=Asia/Hong_Kong
echo run_api_server_with_docker_5555.sh.date=`date`

#small test on 5555
sh -c "sleep 3 && curl http://127.0.0.1:5555/ -d '{m:\"call\",p:[\"SPAPI_GetDllVersion\"]}'" &

sh run_api_server_with_docker.sh "-p5555:4444" -p=4444

