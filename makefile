CC = g++
CFLAGS = -I/usr/local/include/SDL2 -I/usr/local/include/opencv4
LDFLAGS = -lSDL2 -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = main

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
