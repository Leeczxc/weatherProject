/*
 * 程序名：filetoblob.cpp 此程序演示开发框架操作MYSQL数据库
 * author:Leeczxc
 */

#include "_mysql.h"

int main(int argc, char *argv[])
{
    connection conn; // 数据库连接类
    // 登录数据库，返回值：0-成功；其他是失败，存放了Mysql的数据代码
    // 失败代码存放在conn.m_cda结构体里面
    if (conn.connecttodb("127.0.0.1,root,alichao1,test,3306", "utf8mb4") != 0)
    {
        printf("connect database failed.\n%s\n", conn.m_cda.message);
        return -1;
    }

    /*
  注意事项：
  1、参数的序号从1开始，连续、递增，参数也可以用问号表示，但是，问号的兼容性不好，不建议；
  2、SQL语句中的右值才能作为参数，表名、字段名、关键字、函数名等都不能作为参数；
  3、参数可以参与运算或用于函数的参数；
  4、如果SQL语句的主体没有改变，只需要prepare()一次就可以了；
  5、SQL语句中的每个参数，必须调用bindin()绑定变量的地址；
  6、如果SQL语句的主体已改变，prepare()后，需重新用bindin()绑定变量；
  7、prepare()方法有返回值，一般不检查，如果SQL语句有问题，调用execute()方法时能发现；
  8、bindin()方法的返回值固定为0，不用判断返回值；
  9、prepare()和bindin()之后，每调用一次execute()，就执行一次SQL语句，SQL语句的数据来自被绑定变量的值。
*/
    // 操作SQL语句的对象
    sqlstatement stmt(&conn);

    // 准备创建表的SQL语句
    // 超女表girls，超女编号id，超女名字name，体重weight，报名时间btime，超女说明memo，图片pic
    struct st_girls
    {
        long id;               // 编号
        char pic[1000000];     // 图片的内容
        unsigned long picsize; // 图片内容占用的字节数
        // char names[31]; // 名字
        // double weight;  // 体重
        // char btime[20]; // 时间
    } stgirls;

    // stmt.prepare("\
    // insert into girls(id,name,weight,btime) values(:1,:2,:3,str_to_date(:4,'%%Y-%%m-%%d %%H:%%i:%%s'))");
    stmt.prepare("update girls set pic=:1 where id=:2");
    // stmt.bindin(1, &stgirls.id);
    stmt.bindinlob(1, stgirls.pic, &stgirls.picsize);
    stmt.bindin(2, &stgirls.id);
    // stmt.bindin(2, stgirls.names, 30);
    // stmt.bindin(3, &stgirls.weight);
    // stmt.bindin(4, stgirls.btime, 19);

    // 模拟数据，向表中插入五条数测试数据
    for (int ii = 1; ii < 3; ++ii)
    {
        memset(&stgirls, 0, sizeof(struct st_girls)); // 结构变量初始化
        stgirls.id = ii;

        // 把图片的内容加载到pic中
        if (ii == 1)
        {
            stgirls.picsize = filetobuf("1.jpg", stgirls.pic);
        }
        if (ii == 2)
        {
            stgirls.picsize = filetobuf("2.jpg", stgirls.pic);
        }
        if (stmt.execute() != 0)
        {
            printf("stmt.execute() %d times failed.\n%s\n%s\n", ii, stmt.m_sql, stmt.m_cda.message);
            return -1;
        }
    }
    printf("insert table girls photo ok.\n");
    conn.commit();
    return 0;
}