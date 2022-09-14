CC := gcc
EXE_PATH := ./exe
SRCS_PATH := ./src

.PHYON: rpro_server rclient rth_server

rclient:
	@$(CC) $(SRCS_PATH)/client.c -o $(EXE_PATH)/client
	@./$(EXE_PATH)/client

rpro_server:
	@$(CC) $(SRCS_PATH)/pro_server.c -o $(EXE_PATH)/pro_server
	@./$(EXE_PATH)/pro_server

rth_server:
	@$(CC) $(SRCS_PATH)/th_server.c -o $(EXE_PATH)/th_server -l pthread
	@./$(EXE_PATH)/th_server