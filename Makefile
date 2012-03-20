XMMS2_PREFIX?=/opt/local

CFLAGS=-Wall -Wextra -Wno-unused-parameter -I$(XMMS2_PREFIX)/include/xmms2 -fdiagnostics-show-option
LDFLAGS=-framework CoreServices -L$(XMMS2_PREFIX)/lib -lxmmsclient -lxmmsclient-cf

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

xmms2-insomnia: xmms2-insomnia.o
	$(CC) -o $@ $^ $(LDFLAGS)

all: xmms2-insomnia

clean:
	rm -f xmms2-insomnia.o xmms2-insomnia
