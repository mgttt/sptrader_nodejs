#	bash -c "source /node_env.sh && export PATH=/\$NODE_VERSION/bin::\$PATH && node -v && node test_ver.js"

#	cmptech/nodejs_sharessl_ubuntu_python:18.04 

if [ ! -n "$DKR_IMAGE" ]; then
	#DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu:18.04
	#DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu
	#DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu_python:18.04
	DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu_python:16.04
#	cmptech/nodejs_sharessl_ubuntu_python_gdb:18.04 \
	echo DKR_IMAGE default to $DKR_IMAGE
fi

#DKR_OPTIONS_MISC=

docker run -v "$PWD:/sptrader" -ti -w /sptrader $DKR_OPTIONS_MISC \
	$DKR_IMAGE bash test_version_inner.sh $*
