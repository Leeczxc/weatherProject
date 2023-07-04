PUBINCL = -I../../public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库
PUBCPP = ../../public/_public.cpp

CFLAGS = -g -o

IMPORT =  ~/Project/weatherProject/public/_ftp.cpp  ~/Project/weatherProject/public/_public.cpp

all: deletefiles gzipfiles procctl checkproc book ftpgetfiles ftpputfiles tcpputfiles fileserver

procctl: procctl.cpp
	g++ -o procctl procctl.cpp
	cp ./procctl ../bin

book: book.cpp
	g++ $(CFLAGS) book book.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp book ../bin/.

checkproc: checkproc.cpp
	g++ $(CFLAGS) checkproc checkproc.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp checkproc ../bin/.

gzipfiles: gzipfiles.cpp
	g++ $(CFLAGS) gzipfiles gzipfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp gzipfiles ../bin/.

deletefiles: deletefiles.cpp
	g++ $(CFLAGS) deletefiles deletefiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp deletefiles ../bin/.

ftpgetfiles: ftpgetfiles.cpp
	g++ $(CFLAGS) ftpgetfiles ftpgetfiles.cpp $(PUBINCL) $(PUBCPP) ~/Project/weatherProject/public/libftp.a ~/Project/weatherProject/public/_ftp.cpp  -lm -lc
	cp ftpgetfiles ../bin/.

ftpputfiles: ftpputfiles.cpp
	g++ $(CFLAGS) ftpputfiles ftpputfiles.cpp $(PUBINCL) $(PUBCPP) ~/Project/weatherProject/public/libftp.a ~/Project/weatherProject/public/_ftp.cpp  -lm -lc
	cp ftpputfiles ../bin/.

tcpputfiles: tcpputfiles.cpp
	g++ $(CFLAGS) tcpputfiles tcpputfiles.cpp $(PUBINCL) $(PUBCPP) ~/Project/weatherProject/public/libftp.a ~/Project/weatherProject/public/_ftp.cpp  -lm -lc
	cp tcpputfiles ../bin/.

fileserver: fileserver.cpp
	g++ $(CFLAGS) fileserver fileserver.cpp $(PUBINCL) $(PUBCPP) ~/Project/weatherProject/public/libftp.a ~/Project/weatherProject/public/_ftp.cpp  -lm -lc
	cp fileserver ../bin/.


clean:
	rm deletefiles
	rm gzipfiles
	rm procctl
	rm book
	rm checkproc
	rm ftpgetfiles
	rm ftpputfiles
	rm tcpputfiles
	rm fileserver