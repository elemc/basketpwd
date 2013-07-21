@ECHO OFF
mkdir build-x86
cd build-x86
call setenv /x86 /Release /xp
cmake -DCMAKE_BUILD_TYPE=Release -DQT_QMAKE_EXECUTABLE=c:\develop\qt\4\x86\bin\qmake.exe -DOPENSSL_ROOT_DIR=c:\develop\openssl-x86 ..
nmake
nmake qt-libs
nmake nsis

cd ..

mkdir build-x64
cd build-x64
call setenv /x64 /Release /xp
cmake -DCMAKE_BUILD_TYPE=Release -DQT_QMAKE_EXECUTABLE=c:\develop\qt\4\x64\bin\qmake.exe -DOPENSSL_ROOT_DIR=c:\develop\openssl-x64 ..
nmake
nmake qt-libs
nmake nsis

cd ..

mkdir build-portable-x86
cd build-portable-x86
call setenv /x86 /Release /xp
cmake -DCMAKE_BUILD_TYPE=Release -DQT_QMAKE_EXECUTABLE=c:\develop\qt\4\x86\bin\qmake.exe -DOPENSSL_ROOT_DIR=c:\develop\openssl-x86 -DPROJECT_PORTABLE=1 ..
nmake
nmake qt-libs
