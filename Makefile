CC = gcc
CFLAGS = -Wall -std=c99
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: marching
clean:
	rm marching
