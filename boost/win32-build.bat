set boost=boost_1_55_0
mkdir include
mkdir include\boost
xcopy %boost%\boost include\boost /c/e/i/f/q/y
mkdir lib
cd %boost%
bjam.exe --build-type=complete --toolset=gcc
bjam.exe --build-type=complete --toolset=msvc-12.0
xcopy stage\lib ..\lib /c/e/i/f/q/y
pause
