CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lm
BIN=/tmp/ge7_bin
EXEC=out
EXEC:=$(BIN)/$(EXEC)
SRC=$(wildcard *.c)
# put any c file that sould not be included in compilation here
SRC:=$(filter-out tests.c makecounter.c,$(SRC))
OBJ=$(SRC:%.c=%.o)
OBJ:=$(OBJ:%=$(BIN)/%)

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
	rm -rf $(BIN)/test
	mkdir -p $(BIN)/test
	sed < tests.c -E 's/^\/\/\% *//g' > $(BIN)/test/.tests.c
	./make_test.sh $(BIN) $(CC)

clean:
	rm -rf $(BIN)

.PHONY: all clean
