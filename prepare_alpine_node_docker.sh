echo "LATEST/V10LTS/V8LTS (copy and run)":
cat <<EE
NODE_VERSION_NUM=\`wget -qO- https://nodejs.org/dist/latest/ | sed -nE 's|.*>node-(.*)\.pkg</a>.*|\1|p'\` && echo \$NODE_VERSION_NUM && \\
wget -c https://nodejs.org/dist/\$NODE_VERSION_NUM/node-\$NODE_VERSION_NUM-linux-x64.tar.gz && \\
tar xzvf node-\$NODE_VERSION_NUM-linux-x64.tar.gz -C alpine_node_latest/ && \\
cd alpine_node_latest/ && rm -f node npm && ln -s \$NODE_VERSION_NUM-linux-x64/bin/node node && ln -s \$NODE_VERSION_NUM-linux-x64/bin/npm npm && cd .. && ls -al alpine_node_latest/
# docker run -ti -v \$PWD:/work/ -w /work/ cmptech/auto_alpine_nodejs_baseline /work/alpine_node_latest/node-\$NODE_VERSION_NUM-linux-x64/bin/node -p process.versions
docker run -ti -v \$PWD:/work/ -w /work/ cmptech/auto_alpine_glibc /work/alpine_node_latest/node-\$NODE_VERSION_NUM-linux-x64/bin/node -p process.versions
NODE_VERSION_NUM=\`wget -qO- https://nodejs.org/dist/latest-v8.x/ | sed -nE 's|.*>node-(.*)\.pkg</a>.*|\1|p'\` && wget -c https://nodejs.org/dist/\$NODE_VERSION_NUM/node-\$NODE_VERSION_NUM.tar.gz
NODE_VERSION_NUM=\`wget -qO- https://nodejs.org/dist/latest-v10.x/ | sed -nE 's|.*>node-(.*)\.pkg</a>.*|\1|p'\` && wget -c https://nodejs.org/dist/\$NODE_VERSION_NUM/node-\$NODE_VERSION_NUM.tar.gz
EE

echo test latest:

# docker run -ti -v $PWD:/work/ -w /work/ cmptech/auto_alpine_nodejs_baseline sh -c "/work/alpine_node_latest/node -p 'process.versions'"
docker run -ti -v $PWD:/work/ -w /work/ cmptech/auto_alpine_glibc /work/alpine_node_latest/node -p process.versions
