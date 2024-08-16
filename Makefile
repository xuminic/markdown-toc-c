TARGET = md-toc

all:
	gcc -Wall -O3 -o $(TARGET) markdown-toc.c

clean:
	rm $(TARGET)

install:
	cp -f $(TARGET) ~/bin
