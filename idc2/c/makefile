# 开发框架头文件路径
PUBINCL = -I../../public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库
PUBCPP = ../../public/_public.cpp

# mysql头文件存放的目录。
MYSQLINCL = -I/usr/include/mysql -I../../public/db/mysql_lee

# mysql库文件存放的目录。
MYSQLLIB = -L/usr/lib

# 需要链接的mysql库。
MYSQLLIBS = -lmysqlclient

MYSQLCPP = ../../public/db/mysql_lee/_mysql.cpp

# 编译参数
CFLAGS = -g

# all: crtsurfdata crtsurfdata1 crtsurfdata2 crtsurfdata3 crtsurfdata4 crtsurfdata5 obtcodetodb
all: crtsurfdata obtcodetodb obtmindtodb

crtsurfdata: crtsurfdata.cpp
	g++ $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata ../bin/.

# crtsurfdata1: crtsurfdata1.cpp
# 	g++ $(CFLAGS) crtsurfdata1 crtsurfdata1.cpp $(PUBINCL) $(PUBCPP) -lm -lc
# 	cp crtsurfdata1 ../bin/.

# crtsurfdata2: crtsurfdata2.cpp
# 	g++ $(CFLAGS) crtsurfdata2 crtsurfdata2.cpp $(PUBINCL) $(PUBCPP) -lm -lc
# 	cp crtsurfdata2 ../bin/.

# crtsurfdata3: crtsurfdata3.cpp
# 	g++ $(CFLAGS) crtsurfdata3 crtsurfdata3.cpp $(PUBINCL) $(PUBCPP) -lm -lc
# 	cp crtsurfdata3 ../bin/.

# crtsurfdata4: crtsurfdata4.cpp
# 	g++ $(CFLAGS) crtsurfdata4 crtsurfdata4.cpp $(PUBINCL) $(PUBCPP) -lm -lc
# 	cp crtsurfdata4 ../bin/.

# crtsurfdata5: crtsurfdata5.cpp
# 	g++ $(CFLAGS) crtsurfdata5 crtsurfdata5.cpp $(PUBINCL) $(PUBCPP) -lm -lc
# 	cp crtsurfdata5 ../bin/.

obtcodetodb: obtcodetodb.cpp
	g++ $(CFLAGS) -o obtcodetodb obtcodetodb.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp obtcodetodb ../bin/.

obtmindtodb: obtmindtodb.cpp
	g++ $(CFLAGS) -o obtmindtodb obtmindtodb.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp obtmindtodb ../bin/.

clean:
	rm crtsurfdata
	# rm crtsurfdata1
	# rm crtsurfdata2
	# rm crtsurfdata3
	# rm crtsurfdata4
	# rm crtsurfdata5
	rm obtcodetodb
	rm obtmindtodb