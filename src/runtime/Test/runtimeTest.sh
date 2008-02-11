#!/bin/sh

BASEREP=`pwd`
OMNIORB_CONFIG=${BASEREP}/omniorb.cfg
OMNINAMES_LOGDIR=${BASEREP}/omnilog

export BASEREP
export OMNIORB_CONFIG
export OMNINAMES_LOGDIR

echo ${BASEREP}
echo ${OMNIORB_CONFIG}

# do not use the default port 2810 for omninames (to improve, cf. SALOME)
echo "InitRef = NameService=corbaname::localhost:2910" > ${OMNIORB_CONFIG}

rm -rf ${OMNINAMES_LOGDIR}
mkdir  ${OMNINAMES_LOGDIR}

echo $$

omniNames -start 2910 &
pidomni=$!
echo $pidomni

#wait enough time to let omniNames start
sleep 2

./echoSrv &
pidecho=$!
echo $pidecho

mkdir -p lib/salome
cp .libs/libTestComponentLocal.so lib/salome
export TESTCOMPONENT_ROOT_DIR=`pwd`

#wait enough time to let echoSrv start and register
sleep 2

./TestRuntime
ret=$?

kill -9 $pidecho $pidomni
cat /tmp/${USER}/UnitTestsResult
exit $ret
