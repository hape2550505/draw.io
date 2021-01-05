server: main.c ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h 
	g++  -o server main.c ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h  -lpthread -lmysqlclient -g

clean:
	rm  -r server
