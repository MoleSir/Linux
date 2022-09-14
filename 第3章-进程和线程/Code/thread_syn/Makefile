CC := gcc
SRCS_PATH := ./src
EXE_PATH := ./exe


.PHYON: rnsyn rmutex rrwlock

rnsyn:
	@$(CC) $(SRCS_PATH)/tnsyn.c -o $(EXE_PATH)/nsyn -l pthread
	@./$(EXE_PATH)/nsyn

rmutex:
	@$(CC) $(SRCS_PATH)/tmutex.c -o $(EXE_PATH)/mutex -l pthread
	@./$(EXE_PATH)/mutex

rrwlock:
	@$(CC) $(SRCS_PATH)/trwlock.c -o $(EXE_PATH)/rwlock -l pthread
	@./$(EXE_PATH)/rwlock