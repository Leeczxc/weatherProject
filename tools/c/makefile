PUBINCL = -I../../public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库
PUBCPP = ../../public/_public.cpp

CFLAGS = -g -o
all: procctl checkproc book

procctl: procctl.cpp
	g++ -o procctl procctl.cpp
	cp ./procctl ../bin

book: book.cpp
	g++ $(CFLAGS) book book.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp book ../bin/.

checkproc: checkproc.cpp
	g++ $(CFLAGS) checkproc checkproc.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp checkproc ../bin/.
clean:
	rm procctl
	rm book
	rm checkproc