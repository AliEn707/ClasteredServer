GCC ?= gcc
CFLAGS= -Wall -fsigned-char -fgnu89-inline 
CPPFLAGS= -Wall -fsigned-char 
LDFLAGS= -pthread -lpthread -lm -lstdc++
SRC=src
SHARE_SOURCES:=$(wildcard $(SRC)/share/*.c) $(wildcard $(SRC)/share/*/*.c) $(wildcard $(SRC)/share/*/*/*.c)
SHARE_OBJECTS:=$(SHARE_SOURCES:.c=.o)

STORAGE?=TEXT

DEFINES:= -DSTORAGE_$(STORAGE)

PUBLIC:=master
PUBLIC_SOURCES=$(wildcard $(SRC)/$(PUBLIC)/*.c) $(wildcard $(SRC)/$(PUBLIC)/*/*.c) $(wildcard $(SRC)/$(PUBLIC)/*/*/*.c)
PUBLIC_OBJECTS=$(PUBLIC_SOURCES:.c=.o)

TEST:=test
TEST_SOURCES=$(wildcard $(SRC)/$(TEST)/*.c) $(wildcard $(SRC)/$(TEST)/*/*.c) $(wildcard $(SRC)/$(TEST)/*/*/*.c)
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)

ifeq ($(DEBUG),1)
    CFLAGS +=-g -ggdb -rdynamic
    CPPFLAGS +=-g -ggdb -rdynamic
endif

ifeq ($(OPTIMISATION),1)
    CFLAGS +=-O3 -ffast-math -fgcse-sm -fgcse-las -fgcse-after-reload -flto -funroll-loops
    CPPFLAGS +=-O3 -ffast-math -fgcse-sm -fgcse-las -fgcse-after-reload -flto -funroll-loops
endif

all: $(SHARE_SOURCES) $(PUBLIC_SOURCES) $(PUBLIC)
	
$(PUBLIC): $(SHARE_OBJECTS) $(PUBLIC_OBJECTS)
	$(GCC) $(LDFLAGS) $(SHARE_OBJECTS) $(PUBLIC_OBJECTS) -o $@

$(TEST): $(SHARE_OBJECTS) $(TEST_OBJECTS) 
	$(GCC) $(LDFLAGS) $(SHARE_OBJECTS) $(TEST_OBJECTS) -o $@

%.o: %.c
	$(GCC) -c $(CFLAGS) $(DEFINES) $< -o $@
	
generator: 
	$(GCC) $(SRC)/other/password_generator.c $(SRC)/share/md5.c $(SRC)/share/base64.c -o generator
fast: $(PUBLIC)_fast
	
$(PUBLIC)_fast:
	$(GCC) $(LDFLAGS) $(CFLAGS) $(SHARE_SOURCES) $(PUBLIC_SOURCES) -o $(PUBLIC)

clean:
	rm -rf $(SHARE_OBJECTS) $(PUBLIC_OBJECTS) $(TEST_OBJECTS) $(PUBLIC) $(TEST) $(PUBLIC).exe $(TEST).exe generator.exe generator