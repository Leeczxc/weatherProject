PUBINCL = -I../public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库
PUBCPP = ../public/_public.cpp

all: procctl book

procctl: procctl.cpp
	g++ -o procctl procctl.cpp
	cp ./procctl ../bin

book: book.cpp
	g++ -o book book.cpp
	cp ./book ../bin
clean:
	rm procctl
	rm book