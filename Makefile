obj-m += stack.o

all:
	make -C ${KERNEL_PATH} M=$(PWD) modules CFLAGS_MODULE=-fno-pic

clean:
	make -C $(KERNEL_PATH) M=$(PWD) clean
