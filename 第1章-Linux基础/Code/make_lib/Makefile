CC := gcc
CFLAGS := -I ./include
SRC_PATH := ./src

.PHYON: buildo builddo buildso

buildo:
	$(CC) $(SRC_PATH)/add.c $(SRC_PATH)/div.c $(SRC_PATH)/mul.c $(SRC_PATH)/sub.c -c $(CFLAGS)


builddo:
	$(CC) $(SRC_PATH)/add.c $(SRC_PATH)/div.c $(SRC_PATH)/mul.c $(SRC_PATH)/sub.c -c -fpic $(CFLAGS)

builddlib:
	$(CC) -shared add.o div.o mul.o sub.o -o libcalc.so