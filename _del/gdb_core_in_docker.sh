echo docker run -ti -v$PWD:/sptrader -w/sptrader cmptech/auto_ubuntuessential_nodejs_sharessl_gdb sh gdb_core_in_docker.sh
. /node_env.sh
gdb node core
