CPPFLAGS := -I ./src/include
LIB_PATH := ./src/lib
LIBS := -l pthread -L $(LIB_PATH) -l tcpcpp

SRC_PATH := ./src
TEST_PATH := ./src/test
OBJ_PATH := ./bin/obj
EXE_PATH := ./bin/exe

.PHYON: buildlib rclient rserver build
 
buildlib:
	@g++ -c $(CPPFLAGS) $(SRC_PATH)/tcp_client.cpp -o $(OBJ_PATH)/tcp_client.o
	@g++ -c $(CPPFLAGS) $(SRC_PATH)/tcp_server.cpp -o $(OBJ_PATH)/tcp_server.o
	@g++ -c $(CPPFLAGS) $(SRC_PATH)/tcp_socket.cpp -o $(OBJ_PATH)/tcp_socket.o
	@g++ -c $(CPPFLAGS) $(SRC_PATH)/tcp_concurrent.cpp -o $(OBJ_PATH)/tcp_concurrent.o
	@ar rcs ./src/lib/libtcpcpp.a $(OBJ_PATH)/tcp_client.o $(OBJ_PATH)/tcp_server.o $(OBJ_PATH)/tcp_socket.o $(OBJ_PATH)/tcp_concurrent.o


rclient:
	@./$(EXE_PATH)/client

rserver:
	@./$(EXE_PATH)/server

build:
	@g++ -c $(CPPFLAGS) $(TEST_PATH)/server.cpp -o $(OBJ_PATH)/server.o -l pthread
	@g++ -c $(CPPFLAGS) $(TEST_PATH)/client.cpp -o $(OBJ_PATH)/client.o
	@g++ $(TEST_PATH)/client.cpp -o $(EXE_PATH)/client $(CPPFLAGS) $(LIBS)
	@g++ $(TEST_PATH)/server.cpp -o $(EXE_PATH)/server $(CPPFLAGS) $(LIBS)
