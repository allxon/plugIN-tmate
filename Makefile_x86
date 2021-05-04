# Makefile example. Compile all .c files and build execution file
QUIET = @
ECHO  = echo
RM = rm -rf

CC := g++

GCC_GXX_WARNINGS =  -Wall -Wno-error -Wno-packed -Wpointer-arith -Wredundant-decls -Wstrict-aliasing=3 -Wswitch-enum -Wundef -Wwrite-strings -Wextra -Wno-unused-parameter \

CFLAGS  = -Os
LDFLAGS = -lm

TARGET = tmate_plugIN
OUTPUTPATH = $(PWD)/output
UTIL_FOLDER = $(PWD)/Util
MAIN_FOLDER = $(PWD)/MainSrc
PLUGINS_FOLDER = $(PWD)/Plugins
ALLOBJS = $(wildcard $(OUTPUTPATH)/*.o)
WEBSOCKET_FOLDER = $(PWD)/websocket

CLIB = $(WEBSOCKET_FOLDER)/lib/libboost_system.a \
	$(WEBSOCKET_FOLDER)/lib/libboost_chrono.a \
	$(WEBSOCKET_FOLDER)/lib/libboost_random.a \
	$(PWD)/lib/libadmplugin.so \
	$(PWD)/lib/libargon2.a
CLIB += -lrt -lpthread -lcurl -lcrypto

BUILD_INFO_INCLUDE_FILE = $(PWD)/Util/include/build_info.h
BUILD_DATE := $(shell date '+%Y%m%d-%H%M%S')
BUILD_VERSION := '1.00.2000'

all: prestep default

prestep:
ifneq ($(wildcard $(OUTPUTPATH)),)
	@echo "Found $(OUTPUTPATH)"
else
	@echo "Did not find $(OUTPUTPATH)"
	mkdir $(OUTPUTPATH)
endif
	$ rm -f $(BUILD_INFO_INCLUDE_FILE) 
	$ echo '#define BUILD_INFO "'$(BUILD_VERSION)_$(BUILD_DATE)'"' > $(BUILD_INFO_INCLUDE_FILE); 

default: $(TARGET)

$(TARGET): compileutil plugins mainsrc
	@echo ""
	@echo "======> Start build $(TARGET) <======"
	$(CC) $(ALLOBJS) $(CLIB) -o $@
	@echo "****** Copy $@ to $(OUTPUTPATH) ******"
	$ mv $@ $(OUTPUTPATH)/

compileutil:
	@echo ""
	@echo "======> Start compile util folder <======"
	$ cd $(UTIL_FOLDER) && make

	@echo "****** Copy *.o to $(OUTPUTPATH) ******"
	$ cd $(OUTPUTPATH) && mv $(UTIL_FOLDER)/src/*.o .

plugins:
	@echo ""
	@echo "======> Start compile samples folder <======"
	$ cd $(PLUGINS_FOLDER) && make

	@echo "****** Copy *.o to $(OUTPUTPATH) ******"
	$ cd $(OUTPUTPATH) && mv $(PLUGINS_FOLDER)/*.o .

mainsrc:
	@echo ""
	@echo "======> Start compile main folder <======"
	$ cd $(MAIN_FOLDER) && make

	@echo "****** Copy *.o to $(OUTPUTPATH) ******"
	$ cd $(OUTPUTPATH) && mv $(MAIN_FOLDER)/*.o .

clean:
	$(QUIET)$(RM) $(OUTPUTPATH)/*
