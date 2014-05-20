cmake ../src -G "MinGW Makefiles" -DCMAKE_CXX_FLAGS="-std=c++11 -D_WIN32_WINNT=0x0501 -DWINVER=0x0501" 
mingw32-make.exe
pause
