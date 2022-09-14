CC := gcc
EXE_PATH := ./exe
SRCS_PATH := ./src

.PHYON: rpro_lp_1 rpro_lp_2 rexecl rno_par rzp rwait

rpro_lp_1:
	@$(CC) $(SRCS_PATH)/pro_loop_1.c -o $(EXE_PATH)/pro_loop_1
	@./$(EXE_PATH)/pro_loop_1

rpro_lp_2:
	@$(CC) $(SRCS_PATH)/pro_loop_2.c -o $(EXE_PATH)/pro_loop_2
	@./$(EXE_PATH)/pro_loop_2

rexecl:
	@$(CC) $(SRCS_PATH)/texecl.c -o $(EXE_PATH)/texecl
	@./$(EXE_PATH)/texecl

rno_par:
	@$(CC) $(SRCS_PATH)/no_par.c -o $(EXE_PATH)/no_par
	@./$(EXE_PATH)/no_par

rzp:
	@$(CC) $(SRCS_PATH)/tzpro.c -o $(EXE_PATH)/tzpro
	@./$(EXE_PATH)/tzpro

rwait:
	@$(CC) $(SRCS_PATH)/twait.c -o $(EXE_PATH)/twait
	@./$(EXE_PATH)/twait