@echo off
cd /d C:\Users\user-\OneDrive\Documents\GitHub\SQL_Laboratory

echo Cleaning...
del *.o 2>nul
del DatabaseGUI.exe 2>nul

echo Compiling sqlite3.c...
gcc -c C:/SQLite/sqlite3.c -o sqlite3.o -O2 -w

echo Compiling C++ files...
g++ -std=c++17 -c src/*.cpp -Iinclude -I"C:/wxWidgets/include" -I"C:/wxWidgets/lib/gcc810_x64_dll/mswu" -I"C:/SQLite" -DWXUSINGDLL -D__WXMSW__ -D__GNUWIN32__ -D_UNICODE

echo Linking...
g++ -o DatabaseGUI.exe *.o -L"C:/wxWidgets/lib/gcc810_x64_dll" -lwxbase32u -lwxmsw32u_core

echo Copying DLLs...
copy "C:\wxWidgets\lib\gcc810_x64_dll\wxbase32u_gcc810.dll" .
copy "C:\wxWidgets\lib\gcc810_x64_dll\wxmsw32u_core_gcc810.dll" .

echo Done! Run DatabaseGUI.exe