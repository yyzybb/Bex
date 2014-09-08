
# make config.mk

CONF=config.mk
echo GCC=g++ > ${CONF}
echo INCLUDEPATH=-I../main -I../../src -I../../../boost/include >> ${CONF}
echo CXXFLAGS=-std=c++11 -g -O0 -DDEBUG -D_DEBUG '${INCLUDEPATH}' >> ${CONF}
echo DYNAMIC_LINK=-L../../../boost/lib -l"pthread" -L"/usr/lib/i386-linux-gnu" -l"gnutls-openssl" -l"ssl" -l"crypto" >> ${CONF}
echo STATIC_LINK=../../../boost/lib/libboost_date_time.a ../../../boost/lib/libboost_filesystem.a ../../../boost/lib/libboost_regex.a ../../../boost/lib/libboost_system.a ../../../boost/lib/libboost_thread.a ../../../boost/lib/libboost_unit_test_framework.a ../../../boost/lib/libboost_locale.a >> ${CONF}
find .. -name *.cpp | awk 'BEGIN {FS="/"}{print $NF}' | awk 'BEGIN {FS="."} {print "objects+= " $1 ".o"}' >> ${CONF}
ls .. -l | grep ^d | awk '{print "VPATH+=../" $9 ":"}' >> ${CONF}
rm *.d
make -f depend.mk
make
