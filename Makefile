QUIET = @
ECHO  = echo
RM = rm -rf

# ENV = x86

TOOLCHAIN=/build/toolchain/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu
TOOLCHAIN_CC=$(TOOLCHAIN)/bin/aarch64-linux-gnu-gcc

CC = g++
GCC_GXX_WARNINGS = -Wall -Wno-error -Wno-packed -Wpointer-arith -Wredundant-decls -Wstrict-aliasing=3 -Wswitch-enum -Wundef -Wwrite-strings -Wextra -Wno-unused-parameter
CFLAGS = -Os -DDEBUG
LDFLAGS = -lm

TARGET = plugIN-tmate
UTIL_FOLDER = $(PWD)/Util
MAIN_FOLDER = $(PWD)/MainSrc
PLUGINS_FOLDER = $(PWD)/Plugins
SCRIPTS_FOLDER = $(PWD)/scripts
ENV_FOLDER = $(PWD)/$(ENV)
OBJ_PATH = $(ENV_FOLDER)/objs
OUTPUTPATH = $(ENV_FOLDER)/output
LIB_FOLDER = $(ENV_FOLDER)/lib
CONFIG_FOLDER = $(ENV_FOLDER)/config
APPGUID_FILE := $(CONFIG_FOLDER)/appGUID
APP_GUID := $(shell cat ${APPGUID_FILE})
INSTALL_FOLDER = $(ENV_FOLDER)/install
BIN_FOLDER = /opt/allxon/plugIN/$(APP_GUID)
TMP_PKG_FOLDER = ./$(TARGET)

CINC = -I$(MAIN_FOLDER) -I$(PWD)/websocket -I$(UTIL_FOLDER)/include -I$(PLUGINS_FOLDER)
SRCDIR = Util/src Plugins MainSrc

CLIB = $(LIB_FOLDER)/libadmplugin.a \
	$(LIB_FOLDER)/libargon2.a \
	$(LIB_FOLDER)/libboost_system.a \
	$(LIB_FOLDER)/libboost_chrono.a \
	$(LIB_FOLDER)/libboost_random.a \
	$(LIB_FOLDER)/libssl.a
CLIB += -lrt -lcrypto -lpthread

C_SRCDIR = $(SRCDIR)
C_SOURCES = $(foreach d,$(C_SRCDIR),$(wildcard $(d)/*.c))
C_OBJS = $(patsubst %.c, $(OBJ_PATH)/%.o, $(C_SOURCES))

CPP_SRCDIR = $(SRCDIR)
CPP_SOURCES = $(foreach d,$(CPP_SRCDIR),$(wildcard $(d)/*.cpp))
CPP_OBJS = $(patsubst %.cpp, $(OBJ_PATH)/%.o, $(CPP_SOURCES))

ALLOBJS = $(wildcard $(OBJ_PATH)/*.o)

BUILD_INFO_INCLUDE_FILE = $(PWD)/Util/include/build_info.h
BUILD_DATE := $(shell date '+%Y%m%d-%H%M%S')
BUILD_VERSION := '1.07.2000'


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
	$(CC) -no-pie $^ -o $(TARGET) $(LDFLAGS) $(CLIB)
	$(QUIET)mkdir -p $(OUTPUTPATH)
	$ mv $(TARGET) $(OUTPUTPATH)/

clean:
	$(QUIET)$(RM) $(OBJ_PATH)
	$(QUIET)rm -f $(OUTPUTPATH)/$(TARGET)

install: $(OUTPUTPATH)/$(TARGET) $(CONFIG_FOLDER) $(SCRIPTS_FOLDER)
ifneq (ls $(BIN_FOLDER),)
	$(QUIET)sudo $(RM) $(BIN_FOLDER)
endif
	$(QUIET)sudo mkdir -p $(BIN_FOLDER)
	$(QUIET)sudo cp $(OUTPUTPATH)/$(TARGET) $(BIN_FOLDER)/
	$(QUIET)sudo cp -r $(CONFIG_FOLDER) $(SCRIPTS_FOLDER) $(BIN_FOLDER)/
	$(QUIET)$(ECHO) "$(TARGET) and config, scripts files are copied to $(BIN_FOLDER)/"

uninstall:
	$(QUIET)sudo $(RM) $(BIN_FOLDER)
	$(QUIET)$(ECHO) "$(TARGET) is removed."
	
toolchainbuild: toolchaininit init compile
	 $(QUIET)$(ECHO) "###### Compile $(CPP_OBJS) $(C_OBJS)done!! ######"

toolchaininit:
	$(eval ENV = jetson)
	$(eval APP_GUID  := $(shell cat $(PWD)/$(ENV)/config/appGUID))
	$(eval CC := $(TOOLCHAIN_CC))
	$(eval CINC := $(CINC) -I$(TOOLCHAIN)/include)
	$(eval CLIB := $(CLIB) -lstdc++)

package: $(OUTPUTPATH)/$(TARGET) $(CONFIG_FOLDER) $(SCRIPTS_FOLDER) $(INSTALL_FOLDER)
ifneq (ls $(TMP_PKG_FOLDER),)
	$(QUIET)$(RM) $(TMP_PKG_FOLDER)
endif
	$(QUIET)mkdir -p $(TMP_PKG_FOLDER)/$(APP_GUID)
	$(QUIET)cp $(OUTPUTPATH)/$(TARGET) $(TMP_PKG_FOLDER)/$(APP_GUID)/
	$(QUIET)cp -r $(CONFIG_FOLDER) $(SCRIPTS_FOLDER) $(TMP_PKG_FOLDER)/$(APP_GUID)/
	$(QUIET)cp $(INSTALL_FOLDER)/uninstall_plugIN.sh $(TMP_PKG_FOLDER)/$(APP_GUID)/
	$(QUIET)cp $(INSTALL_FOLDER)/install_plugIN.sh $(TMP_PKG_FOLDER)/
	$(QUIET)cd $(TMP_PKG_FOLDER); tar -czf $(OUTPUTPATH)/$(TARGET).tar.gz .
	$(QUIET)rm -rf $(TMP_PKG_FOLDER)
	$(QUIET)$(ECHO) "The $(TARGET) app related files are packaged to ./output/$(TARGET).tar.gz"

rebuild: clean default
