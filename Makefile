# Compiler and flags

all: test_interactive test_witness STARK

CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -w 
PKG_CONFIG := pkg-config
OPENSSL_CFLAGS := $(shell $(PKG_CONFIG) --cflags openssl)
OPENSSL_LDFLAGS := $(shell $(PKG_CONFIG) --libs openssl)

# Source files and targets
SRC_DIR := ./test
TARGETS := test_interactive test_witness STARK

test_interactive: $(SRC_DIR)/testStark.cpp
	$(CXX) $(CXXFLAGS) $(OPENSSL_CFLAGS) $< -o $@ $(OPENSSL_LDFLAGS)

test_witness: $(SRC_DIR)/testWitness.cpp
	$(CXX) $(CXXFLAGS) $(OPENSSL_CFLAGS) $< -o $@ $(OPENSSL_LDFLAGS)

STARK: STARK.cpp 
	$(CXX) $(CXXFLAGS) $(OPENSSL_CFLAGS) $< -o $@ $(OPENSSL_LDFLAGS)

.PHONY: all clean

clean:
	rm -f $(TARGETS)
