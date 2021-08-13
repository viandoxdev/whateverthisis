CC=gcc
CFLAGS=-Wall -g #-fsanitize=address
LDFLAGS=-lm #-fsanitize=address
BIN=/tmp/ge7_bin
EXEC=out
EXEC:=$(BIN)/$(EXEC)
SRC=$(wildcard *.c)
# put any c file that sould not be included in compilation here
SRC:=$(filter-out tests.c hash.c makecounter.c,$(SRC))
OBJ=$(SRC:%.c=%.o)
OBJ:=$(OBJ:%=$(BIN)/%)
COMPS=$(wildcard components/*.h)

$(shell mkdir -p $(BIN))

run: all
	@chmod +x $(EXEC)
	@echo "$(EXEC)"
	@echo ""
	@$(EXEC)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

.SECONDEXPANSION:
$(BIN)/%.o: %.c $$(wildcard %.h)
	$(CC) $(CFLAGS) $< -c -o $@

# explicit dependencies
# eg. $(BIN)/main.o: config.h
# ---------------------

tests: tests.c $(OBJ)
	mkdir -p $(BIN)/test/src
	mv $(BIN)/test/src $(BIN)/test/cmp
	mkdir -p $(BIN)/test/src
	sed < tests.c -E 's/^\/\/\% *//g' > $(BIN)/test/.tests.c
	# 5 is the timeout
	./make_test.sh $(BIN) $(CC) 5

components.h: $(COMPS) $(BIN)/hash make_components.sh
	./make_components.sh $(BIN)

$(BIN)/hash: $(BIN)/hash.o $(BIN)/hashmap.o $(BIN)/genvec.o $(BIN)/vector.o
	$(CC) $^ -o $@
$(BIN)/hash.o: hash.c
	$(CC) $< -c -o $@

clean:
	rm -f components.h
	rm -rf $(BIN)

.PHONY: all clean 
