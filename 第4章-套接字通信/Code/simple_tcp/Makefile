CC := gcc
EXE_PATH := ./exe
SRCS_PATH := ./src

.PHYON: rserver rclient

rclient:
	@$(CC) $(SRCS_PATH)/tcp_client.c -o $(EXE_PATH)/client
	@./$(EXE_PATH)/client

rserver:
	@$(CC) $(SRCS_PATH)/tcp_server.c -o $(EXE_PATH)/server
	@./$(EXE_PATH)/server