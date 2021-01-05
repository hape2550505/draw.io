#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "./lock/locker.h"
#include "./threadpool/threadpool.h"
#include "./http/http_conn.h"

#define MAX_FD 65536           //最大文件描述符
#define MAX_EVENT_NUMBER 10000 //最大事件数



//#define listenfdET //边缘触发非阻塞
#define listenfdLT //水平触发阻塞

//这三个函数在http_conn.cpp中定义，改变链接属性
extern int addfd(int epollfd, int fd, bool one_shot);
extern int remove(int epollfd, int fd);
extern int setnonblocking(int fd);

//epollfd
static int epollfd = 0;





int main(int argc, char *argv[])
{


    if (argc <= 1)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    int port = atoi(argv[1]);


    //创建线程池
    threadpool<http_conn> *pool = NULL;
    try
    {
        pool = new threadpool<http_conn>();//参数都是默认实参
    }
    catch (...)
    {
        return 1;
    }

    http_conn *users = new http_conn[MAX_FD];//新建max个并发连接
    assert(users);


    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);



    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(listenfd, 5);
    assert(ret >= 0);

    //创建内核事件表
    epoll_event events[MAX_EVENT_NUMBER];
    epollfd = epoll_create(5);
    assert(epollfd != -1);

    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

	bool stop_server = false;
    while (!stop_server)//主循环
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            printf("%s\n", "epoll failure");
            break;
        }

        for (int i = 0; i < number; i++)//处理新链接+读+写
        {
            int sockfd = events[i].data.fd;

            //处理新到的客户连接
            if (sockfd == listenfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
#ifdef listenfdLT
                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                if (connfd < 0)
                {
                    printf("%s:errno is:%d\n", "accept error", errno);
                    continue;
                }
                if (http_conn::m_user_count >= MAX_FD)//同时连接数过多
                {
					printf("%s\n", "Internal server busy");
                    continue;
                }
                users[connfd].init(connfd, client_address);

#endif

#ifdef listenfdET
                while (1)
                {
                    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                    if (connfd < 0)
                    {
						printf("%s:errno is:%d\n", "accept error", errno);
                        break;
                    }
                    if (http_conn::m_user_count >= MAX_FD)
                    {
                        printf("%s\n", "Internal server busy");
                        break;
                    }
                    users[connfd].init(connfd, client_address);

                }
                continue;
#endif
            }

            //处理客户连接上接收到的数据
            else if (events[i].events & EPOLLIN)
            {
                if (users[sockfd].read_once())//主线程内核控制读写，服务器处理扔给工作线程
                {
					printf("deal with the client(%s)\n", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    //若监测到读事件，将该事件放入请求队列
                    pool->append(users + sockfd);//LT没读完也会放到队列里
                    
                }
            }
            else if (events[i].events & EPOLLOUT)//
            {
				users[sockfd].write();//保证写就一次写完
                
            }
        }
       
    }
    close(epollfd);
    close(listenfd);
    delete[] users;
    delete pool;
    return 0;
}
