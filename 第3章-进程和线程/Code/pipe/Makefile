CC := gcc
SRCS_PATH = ./src
EXE_PATH = ./exe

.PHYON: rpipe rname_pipe

rpipe: 
	@$(CC) $(SRCS_PATH)/tpipe.c -o $(EXE_PATH)/tpipe
	@./$(EXE_PATH)/tpipe

rname_pipe:
	@$(CC) $(SRCS_PATH)/twr_pipe.c -o $(EXE_PATH)/twr_pipe
	@$(CC) $(SRCS_PATH)/trd_pipe.c -o $(EXE_PATH)/trd_pipe
	@./$(EXE_PATH)/twr_pipe $(EXE_PATH)/trd_pipe
