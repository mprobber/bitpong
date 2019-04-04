# the compiler: gcc for C program, define as g++ for C++
CC = lcc

# the build target executable:
TARGET = gb

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET).gb $(TARGET).c

clean:
	$(RM) $(TARGET)