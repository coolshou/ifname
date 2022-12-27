
# x86 cross compile
#CC=i686-linux-gnu-gcc
#STRIP=i686-linux-gnu-strip
# x86_64
CC=gcc
LDFLAGS=-static
STRIP=x86_64-linux-gnu-strip

#raspeberry /opt/cross-pi-gcc/bin/
#https://stackoverflow.com/questions/19162072/how-to-install-the-raspberry-pi-cross-compiler-on-my-linux-host-machine/58559140#58559140
#CC=arm-linux-gnueabihf-gcc
#STRIP=arm-linux-gnueabihf-strip

all:
	${CC} ${LDFLAGS} -o ifname ifname.c
	${STRIP} ifname

clean:
	rm -f ifname
