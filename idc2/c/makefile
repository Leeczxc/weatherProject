# 开发框架头文件路径
PUBINCL = -I../../public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库
PUBCPP = -I../../public/_public.cpp

# 编译参数
CFLAGS = -g -o

all: crtsurfdata1

crtsurfdata1: crtsurfdata1.cpp
	g++ $(CFLAGS) crtsurfdata1 crtsurfdata1.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata1 ../bin/.

clean:
	rm crtsurfdata1
