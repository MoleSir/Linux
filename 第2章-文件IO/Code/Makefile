CC := gcc
SRC_PATH := ./src
EXE_PATH := ./exe

.PHYON: ropen rcreate rcheck rcopy rexpand rerror\
		rstat_getsize rstat_gettype rdup\
		rdup2_copy rdup2_relocate rfcntl

ropen:
	@$(CC) $(SRC_PATH)/open.c -o $(EXE_PATH)/open
	@./$(EXE_PATH)/open

rcreate:
	@$(CC) $(SRC_PATH)/create.c -o $(EXE_PATH)/create
	@./$(EXE_PATH)/create

rcheck:
	@$(CC) $(SRC_PATH)/check.c -o $(EXE_PATH)/check
	@./$(EXE_PATH)/check

rcopy:
	@$(CC) $(SRC_PATH)/copy.c -o $(EXE_PATH)/copy
	@./$(EXE_PATH)/copy

rexpand:
	@$(CC) $(SRC_PATH)/expand.c -o $(EXE_PATH)/expand
	@./$(EXE_PATH)/expand

rerror:
	@$(CC) $(SRC_PATH)/error.c -o $(EXE_PATH)/error
	@./$(EXE_PATH)/error

rstat_getsize:
	@$(CC) $(SRC_PATH)/stat_getsize.c -o $(EXE_PATH)/stat_getsize
	@./$(EXE_PATH)/stat_getsize

rstat_gettype:
	@$(CC) $(SRC_PATH)/stat_gettype.c -o $(EXE_PATH)/stat_gettype
	@./$(EXE_PATH)/stat_gettype

rdup:
	@$(CC) $(SRC_PATH)/tdup.c -o $(EXE_PATH)/tdup
	@./$(EXE_PATH)/tdup

rdup2_copy:
	@$(CC) $(SRC_PATH)/tdup2_copy.c -o $(EXE_PATH)/tdup2_copy
	@./$(EXE_PATH)/tdup2_copy


rdup2_relocate:
	@$(CC) $(SRC_PATH)/tdup2_relocate.c -o $(EXE_PATH)/tdup2_relocate
	@./$(EXE_PATH)/tdup2_relocate

rfcntl:
	@$(CC) $(SRC_PATH)/tfcntl.c -o $(EXE_PATH)/tfcntl
	@./$(EXE_PATH)/tfcntl