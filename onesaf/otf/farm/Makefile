SRC = ..

TARGET_DIR = $(SRC)/$(SWR)/lib/so/$(OS)/$(ARCH)

TEMP_TARGET = $(shell basename $(PWD))

FINAL_TARGET = lib$(TEMP_TARGET)$(LIB_EXT)

SOURCES = $(wildcard *.cpp)

ERC_PATH = .
DEPEND_OBJECTS = $(patsubst %.cpp,$(OUTPUT_DIR)/%.o,$(SOURCES))

include $(SRC)/make.os

INCLUDES = \
	-I . \
	-I $(SRC) 

LIBRARIES_TMP = \
	-L$(LIB_DIR) \
	-lcore 
	
ifneq ($(OS),linux)
ifneq ($(OS),mac)
ifneq ($(ARCH),x64)

INCLUDES += \
	-I /usr/include/w32api 

endif
endif
endif
