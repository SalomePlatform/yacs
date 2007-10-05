#!/bin/sh

killall -9 omniNames echoSrv

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

mkdir -p /tmp/${USER}
\rm -f /tmp/${USER}/UnitTestsResult

echo $$

omniNames -start 2910 &
pidomni=$!
echo $pidomni

./echoSrv &
pidecho=$!
echo $pidecho

export PYTHONPATH=${BASEREP}:${PYTHONPATH}
./.libs/TestStandAlone
ret=$?
# 
# kill -9 $pidecho $pidomni
cat /tmp/${USER}/UnitTestsResult
exit $ret
