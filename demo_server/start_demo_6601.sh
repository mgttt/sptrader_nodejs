SERVER_PORT=6601
sh run_api_server_with_docker.sh "-p$SERVER_PORT:4444" -server_id=$SERVER_PORT -http_port=4444 -approot=/sptrader/ | node handle_stdin_log.js $SERVER_PORT.log
