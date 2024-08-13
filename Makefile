
all:
	gcc -Wall -O3 -o md_toc markdown-toc.c

clean:
	rm md_toc

install:
	cp -f md_toc ~/bin
