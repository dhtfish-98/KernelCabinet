CC ?= clang
CFLAGS ?= -Wall -Wextra -pedantic -std=c99 -O2
PREFIX ?= /usr/local

.PHONY: all clean install
all: kernelcabinet

kernelcabinet: Sources/kc_container_walk.c Sources/kc_export_flow.c Sources/kc_bootstrap.c Sources/kc_image_layout.h
	$(CC) $(CFLAGS) Sources/kc_container_walk.c Sources/kc_export_flow.c Sources/kc_bootstrap.c -o $@

clean:
	$(RM) kernelcabinet

install: kernelcabinet
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 kernelcabinet $(DESTDIR)$(PREFIX)/bin/
