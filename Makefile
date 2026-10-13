BIN = ./bin
INCLUDE = ./include
SRC = ./src
OBJ = ./obj

LIBS = -lm
CC = gcc

OUT = $(BIN)/data_structures.exe

SRC_FILES := $(wildcard $(SRC)/*.c)
UNIT_TEST_FILES := $(wildcard $(TE))
OBJ_FILES := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRC_FILES))

all: $(OBJ_FILES)
	$(CC) ./main.c $(OBJ_FILES) -I $(INCLUDE) -o $(OUT) $(LIBS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -I $(INCLUDE) -o $@

run:
	$(OUT)

clean:
	rm $(OBJ)/*.o
	rm $(BIN)/*.exe