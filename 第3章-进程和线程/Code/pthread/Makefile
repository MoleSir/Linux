CC := gcc
SRCS_PATH := ./src
EXE_PATH := ./exe


.PHYON: rcreate rexit rjoin1 rjoin2 rjoin3 rdetach rtest1

rcreate:
	@$(CC) $(SRCS_PATH)/tcreate.c -o $(EXE_PATH)/create -l pthread
	@./$(EXE_PATH)/create

rexit:
	@$(CC) $(SRCS_PATH)/texit.c -o $(EXE_PATH)/exit -l pthread
	@./$(EXE_PATH)/exit

rjoin1:
	@$(CC) $(SRCS_PATH)/tjoin1.c -o $(EXE_PATH)/join1 -l pthread
	@./$(EXE_PATH)/join1

rjoin2:
	@$(CC) $(SRCS_PATH)/tjoin2.c -o $(EXE_PATH)/join2 -l pthread
	@./$(EXE_PATH)/join2

rjoin3:
	@$(CC) $(SRCS_PATH)/tjoin3.c -o $(EXE_PATH)/join3 -l pthread
	@./$(EXE_PATH)/join3

rdetach:
	@$(CC) $(SRCS_PATH)/tdetach.c -o $(EXE_PATH)/detach -l pthread
	@./$(EXE_PATH)/detach

rtest1:
	@$(CC) $(SRCS_PATH)/test1.c -o $(EXE_PATH)/test1 -l pthread
	@./$(EXE_PATH)/test1