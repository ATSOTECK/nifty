.PHONY: build
build:
	sh build.sh

.PHONY: install
install: build
	sh install.sh

.PHONY: clean
clean:
	rm -rf bin/