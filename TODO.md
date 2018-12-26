# TODO

* pm2 sptrader_pool_mgr.js

```
DIR => monitor => loop check *.sock => ping => monitor(mem).pool

server => launch entry js

entry program => pid => $pid.sock + lock($pid).pid

mem.pool => manually + auto later => cleanup(kill pid and rm pid&sock)

```

* wait for libapiwrapper update to support openssl 1.1
* using nodenodenode pure IPC version
* upgrade to json 3.X and support handling of type_error...
