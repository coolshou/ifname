TARGET=ifname
# x86 cross compile
#CC=i686-linux-gnu-gcc
#STRIP=i686-linux-gnu-strip
# x86_64
LDFLAGS=-static
#CROSS_TOOL=i686-linux-gnu-
CC=$(CROSS_TOOL)gcc
STRIP=$(CROSS_TOOL)strip

#raspeberry /opt/cross-pi-gcc/bin/
#https://stackoverflow.com/questions/19162072/how-to-install-the-raspberry-pi-cross-compiler-on-my-linux-host-machine/58559140#58559140
#CC=arm-linux-gnueabihf-gcc
#STRIP=arm-linux-gnueabihf-strip

all: x64 x86
	#${CC} ${LDFLAGS} -o ${TARGET} ${TARGET}.c
	#${STRIP} ${TARGET}

x64:
	${CC} ${LDFLAGS} -m64 -o ${TARGET} ${TARGET}.c
	mv ${TARGET} amd64/
x86:
	${CC} ${LDFLAGS} -m32 -o ${TARGET} ${TARGET}.c
	mv ${TARGET} x86/

clean:
	rm -f ${TARGET}
