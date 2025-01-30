# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wno-deprecated-declarations

# Detect the operating system
UNAME_S := $(shell uname -s)

# Raylib paths and flags based on the operating system
ifeq ($(UNAME_S),Linux)
    RAYLIB_PATH = /usr/lib/raylib
    INCLUDES = -I/usr/include/raylib
    LDFLAGS = -L$(RAYLIB_PATH) -lraylib -lcrypto -lssl
    # Add any Linux-specific flags here
else ifeq ($(UNAME_S),Darwin)
    # Detect Mac processor architecture
    UNAME_M := $(shell uname -m)
    ifeq ($(UNAME_M),arm64)
        # M1/M2 Mac paths
        RAYLIB_PATH = /opt/homebrew/Cellar/raylib
        OPENSSL_PATH = /opt/homebrew/opt/openssl
    else
        # Intel Mac paths
        RAYLIB_PATH = /usr/local/Cellar/raylib
        OPENSSL_PATH = /usr/local/opt/openssl
    endif
    RAYLIB_INCLUDE = $(shell find $(RAYLIB_PATH) -name include)
    RAYLIB_LIB = $(shell find $(RAYLIB_PATH) -name lib)
    INCLUDES = -I$(RAYLIB_INCLUDE) -I$(OPENSSL_PATH)/include
    LDFLAGS = -L$(RAYLIB_LIB) -L$(OPENSSL_PATH)/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lcrypto -lssl
else ifeq ($(OS),Windows_NT)
    RAYLIB_PATH = C:/raylib
    OPENSSL_PATH = "C:/Program Files/OpenSSL-Win64"
    INCLUDES = -I$(RAYLIB_PATH)/include -I$(OPENSSL_PATH)/include
    LDFLAGS = -L$(RAYLIB_PATH)/lib -L$(OPENSSL_PATH)/lib/VC/x64/MDd -lraylib -lcrypto -lssl -lgdi32 -lwinmm -lws2_32
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

help:
	@echo "Available targets:"
	@echo "  all   - Build the project"
	@echo "  clean - Remove generated files"
