#g++ -c main.cpp -o main.o -std=c++11 -I"../src" -I"../../boost/include" -L"/usr/lib/i386-linux-gnu" -l"gnutls-openssl" -l"ssl" -l"crypto" -l"pthread"
#g++ main.o "../../boost/lib/libboost_system.a" "../../boost/lib/libboost_thread.a" "../../boost/lib/libboost_date_time.a" -static -l"pthread"

g++ -c main.cpp -o main.o -std=c++11 -I"../src" -I"../../boost/include" 
g++ main.o "../../boost/lib/libboost_system.a" "../../boost/lib/libboost_thread.a" "../../boost/lib/libboost_date_time.a" -l"pthread" -L"/usr/lib/i386-linux-gnu" -l"gnutls-openssl" -l"ssl" -l"crypto" -l"pthread" -o test_bexio.out

