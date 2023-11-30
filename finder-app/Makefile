#CROSS_COMPILE=aarch64-none-linux-gnu-
CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc
TARGET=writer

all: $(TARGET)
 
$(TARGET): $(TARGET).o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -Wall -c $<
     
clean:
	rm -f $(TARGET) $(TARGET).o
