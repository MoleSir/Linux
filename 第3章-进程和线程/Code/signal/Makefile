CC := gcc
EXE_PATH := ./exe
SRCS_PATH := ./src

.PHYON: ralarm rsignal rsigac

ralarm:
	@$(CC) $(SRCS_PATH)/talarm.c -o $(EXE_PATH)/talarm
	@./$(EXE_PATH)/talarm 

rsignal:
	@$(CC) $(SRCS_PATH)/tsignal.c -o $(EXE_PATH)/tsignal
	@./$(EXE_PATH)/tsignal

rsigac:
	@$(CC) $(SRCS_PATH)/tsigaction.c -o $(EXE_PATH)/tsigac
	@./$(EXE_PATH)/tsigac
	