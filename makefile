# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Detect the operating system
UNAME_S := $(shell uname -s)

# Raylib paths and flags based on the operating system
ifeq ($(UNAME_S),Linux)
    RAYLIB_PATH = /usr/lib/raylib
    INCLUDES = -I/usr/include/raylib
    LDFLAGS = -L$(RAYLIB_PATH) -lraylib -lcrypto -lssl
    # Add any Linux-specific flags here
else ifeq ($(UNAME_S),Darwin)
    RAYLIB_PATH = /opt/homebrew/Cellar/raylib
    RAYLIB_INCLUDE = $(shell find $(RAYLIB_PATH) -name include)
    RAYLIB_LIB = $(shell find $(RAYLIB_PATH) -name lib)
    INCLUDES = -I$(RAYLIB_INCLUDE)
    LDFLAGS = -L$(RAYLIB_LIB) -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lcrypto -lssl
else ifeq ($(OS),Windows_NT)
    RAYLIB_PATH = C:/raylib
    INCLUDES = -I$(RAYLIB_PATH)/include
    LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lcrypto -lssl
    # Add any Windows-specific flags here
else
    $(error Unsupported OS)
endif

# Target
all:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o ui *.cpp $(LDFLAGS)

# Clean
clean:
	rm -f ui
