GCC ?= gcc
CFLAGS= -Wall -fsigned-char -fgnu89-inline 
LDFLAGS= -pthread -lpthread -lm
SRC=src
SHARE_SOURCES:=$(wildcard $(SRC)/share/*.c) $(wildcard $(SRC)/share/*/*.c) $(wildcard $(SRC)/share/*/*/*.c)
SHARE_OBJECTS:=$(SHARE_SOURCES:.c=.o)

DEFINES:= -DSTORAGE_TEXT

PUBLIC:=master
PUBLIC_SOURCES=$(wildcard $(SRC)/$(PUBLIC)/*.c) $(wildcard $(SRC)/$(PUBLIC)/*/*.c) $(wildcard $(SRC)/$(PUBLIC)/*/*/*.c)
PUBLIC_OBJECTS=$(PUBLIC_SOURCES:.c=.o)

TEST:=test
TEST_SOURCES=$(wildcard $(SRC)/$(TEST)/*.c) $(wildcard $(SRC)/$(TEST)/*/*.c) $(wildcard $(SRC)/$(TEST)/*/*/*.c)
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)

ifeq ($(DEBUG),1)
    CFLAGS +=-g -ggdb
endif

ifeq ($(OPTIMISATION),1)
    CFLAGS +=-O3 -ffast-math -fgcse-sm -fgcse-las -fgcse-after-reload -flto
endif

all: $(SHARE_SOURCES) $(PUBLIC_SOURCES) $(PUBLIC)
	
$(PUBLIC): $(SHARE_OBJECTS) $(PUBLIC_OBJECTS)
	$(GCC) $(LDFLAGS) $(SHARE_OBJECTS) $(PUBLIC_OBJECTS) -o $@

$(TEST): $(SHARE_OBJECTS) $(TEST_OBJECTS) 
	$(GCC) $(LDFLAGS) $(SHARE_OBJECTS) $(TEST_OBJECTS) -o $@

%.o: %.c
	$(GCC) -c $(CFLAGS) $(DEFINES) $< -o $@
	
fast: $(PUBLIC)_fast
	
$(PUBLIC)_fast:
	$(GCC) $(LDFLAGS) $(CFLAGS) $(SHARE_SOURCES) $(PUBLIC_SOURCES) -o $(PUBLIC)

clean:
	rm -rf $(SHARE_OBJECTS) $(PUBLIC_OBJECTS) $(TEST_OBJECTS) $(PUBLIC) $(TEST) $(PUBLIC).exe $(TEST).exe