export TZ=Asia/Hong_Kong
echo run_api_server_in_docer_gdb.sh.date=`date`

#http://www.commandlinefu.com/commands/view/6004/print-stack-trace-of-a-core-file-without-needing-to-enter-gdb-interactively
#gdb --batch --quiet -ex "thread apply all bt full" -ex "quit" ${exe} ${corefile}
#gdb -batch -ex "run" -ex "bt" ${my_program} 2>&1 | grep -v ^"No stack."$
#alias gdbbt="gdb -q -n -ex bt -batch"

#. /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so gdb -batch -ex "run" -ex "bt" -args node sptrader_api_server.js $* 2>&1
#. /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so gdb -q -batch -ex "run" -ex "thread apply all bt full" -ex "quit" -args node sptrader_api_server.js $* 2>&1
#ulimit -c unlimited && ulimit -a && . /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so gdb -q -batch -ex "run" -ex "thread apply all backtrace" -ex "generate-core-file core.dump" -ex "quit" -args node sptrader_api_server.js $* 2>&1
#ulimit -c unlimited && ulimit -a && . /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so gdb -q -batch -ex "run" -ex "thread apply all backtrace" -ex "generate-core-file coredump" -ex "quit" -args node sptrader_api_server.js $* 2>&1
ulimit -c unlimited && ulimit -a && . /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so gdb -q -batch -ex "run" -ex "thread apply all backtrace" -ex "quit" -args node --expose_gc sptrader_api_server.js $* 2>&1
