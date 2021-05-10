QUIET = @
ECHO  = echo
RM = rm -rf

CC = g++
GCC_GXX_WARNINGS = -Wall -Wno-error -Wno-packed -Wpointer-arith -Wredundant-decls -Wstrict-aliasing=3 -Wswitch-enum -Wundef -Wwrite-strings -Wextra -Wno-unused-parameter
CFLAGS = -Os
LDFLAGS = -lm

APP_GUID = 286b0652-c5ef-46c0-aa8c-7b617bbf6ab9
TARGET = tmate_plugIN
OBJ_PATH = objs
OUTPUTPATH = $(PWD)/output
UTIL_FOLDER = $(PWD)/Util
MAIN_FOLDER = $(PWD)/MainSrc
PLUGINS_FOLDER = $(PWD)/Plugins
CONFIG_FOLDER = $(PWD)/config
SCRIPTS_FOLDER = $(PWD)/scripts
BIN_FOLDER = /opt/allxon/plugINs/$(APP_GUID)

CINC = -I$(MAIN_FOLDER) -I$(PWD)/websocket -I$(UTIL_FOLDER)/include -I$(PLUGINS_FOLDER)
SRCDIR = Util/src Plugins MainSrc
CLIB = -lboost_system -lboost_chrono -lboost_random -ladmplugin -lrt -lpthread -lssl -lcrypto
CLIB += $(PWD)/lib/libargon2.a

C_SRCDIR = $(SRCDIR)
C_SOURCES = $(foreach d,$(C_SRCDIR),$(wildcard $(d)/*.c))
C_OBJS = $(patsubst %.c, $(OBJ_PATH)/%.o, $(C_SOURCES))

CPP_SRCDIR = $(SRCDIR)
CPP_SOURCES = $(foreach d,$(CPP_SRCDIR),$(wildcard $(d)/*.cpp))
CPP_OBJS = $(patsubst %.cpp, $(OBJ_PATH)/%.o, $(CPP_SOURCES))

ALLOBJS = $(wildcard $(OBJ_PATH)/*.o)

BUILD_INFO_INCLUDE_FILE = $(PWD)/Util/include/build_info.h
BUILD_DATE := $(shell date '+%Y%m%d-%H%M%S')
BUILD_VERSION := '1.06.2001'


default:init compile
	$(QUIET)$(ECHO) "###### Compile $(CPP_OBJS) $(C_OBJS)done!! ######"

$(C_OBJS):$(OBJ_PATH)/%.o:%.c
	$(QUIET)$(ECHO) "$(CC) $(CFLAGS) -DLINUX $(GCC_GXX_WARNINGS) -c -o2 $(CINC) $< -o $@"
	$(QUIET)$(CC) $(CFLAGS) -DLINUX $(GCC_GXX_WARNINGS) -c -o2 $(CINC) $< -o $@

$(CPP_OBJS):$(OBJ_PATH)/%.o:%.cpp
	$(QUIET)$(ECHO) "$(CC) $(CFLAGS) -DLINUX $(GCC_GXX_WARNINGS) -c -o2 $(CINC) $< -o $@"
	$(QUIET)$(CC) $(CFLAGS) -DLINUX $(GCC_GXX_WARNINGS) -c -o2 $(CINC) $< -o $@

init:
	$(foreach d,$(SRCDIR), mkdir -p $(OBJ_PATH)/$(d);)
	$(ECHO) '#define BUILD_INFO "'$(BUILD_VERSION)_$(BUILD_DATE)'"' > $(BUILD_INFO_INCLUDE_FILE);

compile:$(C_OBJS) $(CPP_OBJS)
	$(CC) $^ -o $(TARGET) $(LDFLAGS) $(CLIB)
	$ mv $(TARGET) $(OUTPUTPATH)/

clean:
	$(QUIET)$(RM) $(OBJ_PATH)
	$(QUIET)rm -f $(OUTPUTPATH)/$(TARGET)

install: $(OUTPUTPATH)/$(TARGET)
	$(QUIET)sudo mkdir $(BIN_FOLDER)
	$(QUIET)sudo cp $(OUTPUTPATH)/$(TARGET) $(BIN_FOLDER)/
	$(QUIET)sudo cp -r $(CONFIG_FOLDER) $(BIN_FOLDER)/
	$(QUIET)sudo cp -r $(SCRIPTS_FOLDER) $(BIN_FOLDER)/
	$(QUIET)$(ECHO) "$(TARGET) and config, scripts files are copied to $(BIN_FOLDER)/"

uninstall:
	$(QUIET)sudo $(RM) $(BIN_FOLDER)
	$(QUIET)$(ECHO) "$(TARGET) is removed."

rebuild: clean compile
