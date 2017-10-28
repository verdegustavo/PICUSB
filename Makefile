all:
	g++ -Wall -O2 main.cpp source/*.cpp -lusb -llogfile -lreadline -Lsource -Iinclude -o bin/Release/pic_lusb
