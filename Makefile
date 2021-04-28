RACK_DIR ?= ../..

# Exported code
FLAGS += -Isrc/genlib -Igen/exports -DGENLIB_NO_JSON
SOURCES += $(wildcard src/genlib/*.cpp)
SOURCES += $(wildcard gen/exports/*.cpp)

# VCV module
FLAGS += -Isrc 
SOURCES += $(wildcard src/*.cpp)

# Include the license and module SVGs in the distribution
DISTRIBUTABLES += $(wildcard ../LICENSE*) res

include $(RACK_DIR)/plugin.mk