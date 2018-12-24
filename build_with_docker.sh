cd `dirname $0`

pwd

#docker run -v"$PWD:/sptrader" -v"$PWD/tmp:/tmp" -ti -w/sptrader $1 \
#	cmptech/auto_ubuntuessential_nodejs_sharessl_python \
#	sh rebuild_in_docker.sh $*

#docker run -v"$PWD:/sptrader" -ti -w/sptrader $1 \
#	cmptech/auto_ubuntuessential_nodejs_sharessl_python \
#	sh build_in_docker.sh $*

# failed...
#	cmptech/nodejs_sharessl_ubuntu_python:18.04 \

if [ ! -n "$DKR_IMAGE" ]; then
	#DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu_python:18.04
	#DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu_python
	DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu_python:16.04
	echo DKR_IMAGE default to $DKR_IMAGE
fi

docker run -v "$PWD:/sptrader" -ti -w /sptrader $1 \
	$DKR_IMAGE sh build_in_docker.sh $*
