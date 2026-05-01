.PHONY: all setup lock configure build run clean

UV := uv
VENV := .venv
PYTHON := $(VENV)/bin/python
BUILD_DIR := build
BUILD_TYPE ?= Release
JOBS ?= $(shell nproc)

all: run

lock:
	$(UV) lock

setup: $(PYTHON)

$(PYTHON): pyproject.toml uv.lock
	$(UV) sync

configure: $(BUILD_DIR)/CMakeCache.txt

$(BUILD_DIR)/CMakeCache.txt: setup CMakeLists.txt
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DPython_EXECUTABLE=$(abspath $(PYTHON))

build: configure
	cmake --build $(BUILD_DIR) --parallel $(JOBS)

run: build
	$(PYTHON) main.py

clean:
	rm -rf $(BUILD_DIR) $(VENV) __pycache__ qml/__pycache__