cd `dirname $0`

pwd

if [ ! -n "$DKR_IMAGE" ]; then
	DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu_python:18.04
	echo DKR_IMAGE default to $DKR_IMAGE
fi

docker run -v "$PWD:/sptrader" -ti -w /sptrader $1 \
	$DKR_IMAGE sh build_in_docker_latest.sh $*
