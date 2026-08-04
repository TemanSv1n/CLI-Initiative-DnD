BUILD_DIR := build

.PHONY: all release debug install uninstall clean

all: release

release:
	cmake -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR)

debug:
	cmake -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR)

install: release
	sudo cmake --install $(BUILD_DIR)

uninstall:
	sudo rm -f /usr/local/bin/cliniti

clean:
	rm -rf $(BUILD_DIR)
