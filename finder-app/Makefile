CROSS_COMPILE=aarch64-none-linux-gnu-
#CROSS_COMPILE=
GCC=$(CROSS_COMPILE)gcc
TARGET=writer

all: $(TARGET)
 
$(TARGET): $(TARGET).o
	$(GCC) -o $@ $<

%.o: %.c
	$(GCC) -Wall -c $<
     
clean:
	rm $(TARGET) $(TARGET).o
