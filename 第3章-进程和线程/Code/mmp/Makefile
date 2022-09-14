CC := gcc
EXE_PATH := ./exe
SRCS_PATH := ./src

.PHYON:	rpar_mmap runp_A runp_B

rpar_mmap:
	@$(CC) $(SRCS_PATH)/relative_mmap.c -o $(EXE_PATH)/relative_mmap
	@echo '=================run================='
	@./$(EXE_PATH)/relative_mmap


runp_A:
	@$(CC) $(SRCS_PATH)/unrelative_proA.c -o $(EXE_PATH)/unrelative_proA
	@./$(EXE_PATH)/unrelative_proA

runp_B:
	@$(CC) $(SRCS_PATH)/unrelative_proB.c -o $(EXE_PATH)/unrelative_proB
	@./$(EXE_PATH)/unrelative_proB