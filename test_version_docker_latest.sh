if [ ! -n "$DKR_IMAGE" ]; then
	DKR_IMAGE=cmptech/nodejs_sharessl_ubuntu:18.04
	echo DKR_IMAGE default to $DKR_IMAGE
fi

docker run -v "$PWD:/sptrader" -ti -w /sptrader $DKR_OPTIONS_MISC \
	$DKR_IMAGE bash test_version_inner_docker.sh $*
