MY_FLAG			:=	-I ./include
SQL_FLAG		:=	-L /usr/lib/mysql -lmysqlclient -I /usr/include/mysql
CPP_FLAG		:=	-Wall -std=c++17


COMMON_F		:=	$(shell find ./common/*)

TARGETS			:=	delete_wasted test_hehepig_file test_hehepig_user test_socket test_hehepig_ufile test_hehepig_sql test_sql

test_socket: ./test/test_socket.cpp $(COMMON_F)
	g++ -o $@ $(CPP_FLAG) $^ $(MY_FLAG) $(SQL_FLAG)

test_delete_wasted: ./test/test_delete_wasted.cpp $(COMMON_F)
	g++ -o $@ $(CPP_FLAG) $^ $(MY_FLAG) $(SQL_FLAG)

test_hehepig_file: ./test/test_hehepig_file.cpp $(COMMON_F)
	g++ -o $@ $(CPP_FLAG) $^ $(MY_FLAG) $(SQL_FLAG)

test_hehepig_user: ./test/test_hehepig_user.cpp $(COMMON_F)
	g++ -o $@ $(CPP_FLAG) $^ $(MY_FLAG) $(SQL_FLAG)

test_hehepig_ufile: ./test/test_hehepig_ufile.cpp $(COMMON_F)
	g++ -o $@ $(CPP_FLAG) $^ $(MY_FLAG) $(SQL_FLAG)

test_hehepig_sql: ./test/test_hehepig_sql.cpp $(COMMON_F)
	g++ -o $@ $(CPP_FLAG) $^ $(MY_FLAG) $(SQL_FLAG)

test_sql: test_sql.cpp
	g++ -o test_sql -Wall test_sql.cpp -L /usr/lib/mysql -lmysqlclient -I /usr/include/mysql


.PHONY: run
run: test_socket
	./test_socket --port 20168 --daemon --logname "log_`date +'%Y%m%d-%H-%M-%S'`.log"

.PHONY: run_fglog
run_fglog: test_socket
	./test_socket --port 20168 --daemon

.PHONY: stop
stop:
	killall test_socket

.PHONY: delete
SLICES_DIR		:=	/var/www/upload
delete: test_delete_wasted
	@for i in `./test_delete_wasted`	;\
	do	\
		echo $$i*	;\
		rm $(SLICES_DIR)/$$i*		;\
	done

.PHONY: clean
clean:
	rm -rf $(TARGETS)
