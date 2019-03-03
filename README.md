# sptrader_nodejs

sptrader nodejs wrapper

NOTES: ubuntu 16.04 passed, 18.04 still error! (related to the openssl version ...)


```
q_sptrader.js

	# lnx
	node test_version /host= /port= /license= /app_id= /user_id= /password=test /host_id=

	# mac (our built docker "cmptech/nodejs_sharessl_ubuntu:16.04")
	docker run -ti -v $PWD:/work/ -w /work/ cmptech/nodejs_sharessl_ubuntu:16.04 sh -c ". /node_env.sh && echo \$NODE_VERSION && LD_PRELOAD=/work/libapiwrapper.so /\$NODE_VERSION/bin/node test_version /host= /port= /license= /app_id= /user_id= /password=test /host_id"
	
```


# NOTES: mac (using docker)

NOTES:
node:latest is 800MB+ which is too big, so we build our own.

## docker images

* our ok docker image: cmptech/auto_ubuntuessential_nodejs_sharessl, which is built from source....
```
https://github.com/cmptech/auto_nodejs_sharessl_ubuntu
https://hub.docker.com/r/cmptech/nodejs_sharessl_ubuntu
```


* alpine with glibc (20 MB / 54 MB)

libapiwrapper.so needs glibc... BUT... as the ssl is not shared, failed !!

```
https://hub.docker.com/r/cmptech/auto_alpine_glibc
https://github.com/cmptech/auto_alpine_glibc
$ sh prepare_alpine_node_docker.sh
docker run -ti -v $PWD:/work/ -w /work/ cmptech/auto_alpine_glibc /work/alpine_node_latest/node -p process.versions
{ node: '11.4.0',
  v8: '7.0.276.38-node.13',
  uv: '1.24.0',
  zlib: '1.2.11',
  ares: '1.15.0',
  modules: '67',
  nghttp2: '1.34.0',
  napi: '3',
  llhttp: '1.0.1',
  http_parser: '2.8.0',
  openssl: '1.1.0j',
  icu: '63.1',
  unicode: '11.0',
  cldr: '34.0',
  tz: '2018e' }

docker run -ti -v $PWD:/work/ -w /work/ cmptech/auto_alpine_glibc sh -c "LD_PRELOAD=/work/libapiwrapper.so /work/alpine_node_latest/node-v8.15.1-linux-x64/bin/node test_version /host= /port= /license= /app_id= /user_id= /password=test /host_id"
[seg fault]

```

* node:alpine - using musl not glibc...(not ok for libapiwrapper.so) (73 MB)

```
docker run -ti node:alpine node -p process.versions

docker run -ti -v $PWD:/work/ -w /work/ cmptech/auto_alpine_glibc sh -c "LD_PRELOAD=/work/libapiwrapper.so /work/alpine_node_latest/node -p \"require('./sptrader.old')()\""
(seg fault)

docker run -ti -v $PWD:/work/ -w /work/ node:alpine sh -c "LD_PRELOAD=/work/libapiwrapper.so node -p \"require('./sptrader.old')()\""
Error loading shared library ld-linux-x86-64.so.2: No such file or directory (needed by /work/libapiwrapper.so)
Error relocating /work/libapiwrapper.so: _ZNSt8numpunctIcE22_M_initialize_numpunctEP15__locale_struct: symbol not found

```

* [deprecated] cmptech/auto_alpine_nodejs_baseline

```

FROM alpine

RUN echo "http://nl.alpinelinux.org/alpine/latest-stable/main" > /etc/apk/repositories \
&& echo "http://nl.alpinelinux.org/alpine/edge/testing/" >> /etc/apk/repositories \
&& echo "http://nl.alpinelinux.org/alpine/edge/community/" >> /etc/apk/repositories \
&& echo "nameserver 8.8.8.8" >> /etc/resolv.conf && apk update \
&& apk add libstdc++ \
&& rm -rf /var/cache/apk/ && mkdir -p /var/cache/apk && rm -rf /tmp/*

```

* [deprecated] alpine node (16MB / 43MB) w/ node runtime but v6
```
# https://hub.docker.com/r/cmptech/auto_alpine_latest_nodejs
# https://github.com/cmptech/auto_alpine_latest_nodejs

$ docker run -ti cmptech/auto_alpine_latest_nodejs node -p process.versions
{ http_parser: '2.7.1',
  node: '6.9.2',
  v8: '5.1.281.88',
  uv: '1.9.1',
  zlib: '1.2.8',
  ares: '1.10.1-DEV',
  icu: '57.1',
  modules: '48',
  openssl: '1.0.2j' }
```

### alpine/node/docker

sh prepare_alpine_node_docker.sh
