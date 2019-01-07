#docker run -ti -v $PWD:/work/ -w /work/ alpine sh
#docker run -ti -v $PWD:/work/ -w /work/ alpine_nodejs_baseline 
#docker run -ti -v $PWD:/work/ -w /work/ alpine_nodejs_baseline sh -c "/work/alpine_node_latest/node -p 'process.versions'"

#wget https://nodejs.org/dist/latest/node-\$NODE_VERSION_NUM-linux-x64.tar.gz && \\

echo LATEST/V10LTS/V8LTS:
cat <<EE
NODE_VERSION_NUM=\`wget -qO- https://nodejs.org/dist/latest/ | sed -nE 's|.*>node-(.*)\.pkg</a>.*|\1|p'\` && \\
wget https://nodejs.org/dist/\$NODE_VERSION_NUM/node-\$NODE_VERSION_NUM-linux-x64.tar.gz && \\
tar xzvf node-\$NODE_VERSION_NUM-linux-x64.tar.gz -C alpine_node_latest/ && \\
docker run -ti -v \$PWD:/work/ -w /work/ cmptech/auto_alpine_nodejs_baseline sh -c "/work/alpine_node_latest/node-\$NODE_VERSION_NUM-linux-x64/bin/node -p 'process.versions'"
NODE_VERSION_NUM=\`wget -qO- https://nodejs.org/dist/latest-v8.x/ | sed -nE 's|.*>node-(.*)\.pkg</a>.*|\1|p'\` && wget https://nodejs.org/dist/\$NODE_VERSION_NUM/node-\$NODE_VERSION_NUM.tar.gz
NODE_VERSION_NUM=\`wget -qO- https://nodejs.org/dist/latest-v10.x/ | sed -nE 's|.*>node-(.*)\.pkg</a>.*|\1|p'\` && wget https://nodejs.org/dist/\$NODE_VERSION_NUM/node-\$NODE_VERSION_NUM.tar.gz
EE
docker run -ti -v $PWD:/work/ -w /work/ cmptech/auto_alpine_nodejs_baseline sh -c "/work/alpine_node_latest/node -p 'process.versions'"
