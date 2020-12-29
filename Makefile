PROJECT      := github.com/amazingchow/over-the-cx-problem
SRC          := $(shell find . -type f -name '*.go' -not -path "./vendor/*")
TARGETS      := ws-server ws-client
ALL_TARGETS  := $(TARGETS)

all: build

build: $(ALL_TARGETS)

$(TARGETS): $(SRC)
	go build $(GOMODULEPATH)/$(PROJECT)/handle-1m-websocket/$@

clean:
	rm -f $(ALL_TARGETS)

.PHONY: all build clean