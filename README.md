# markdown-toc-c
This filter program generates a Table of Contents (TOC) from Markdown files.
Written in C with standard libraries, making it light, fast and 
highly portable across various systems.

## Installation
Clone the repo:
```
git clone https://github.com/xuminic/markdown-toc-c
```

Build the program:
```
make
```

## Quick Start
- filtering from pipe:
  ```
  cat README.md | md-toc
  ```
  The TOC will be printed out in the console.

- read from files:
  ```
  md-toc README.md
  ```
  The TOC will be printed out in the console.

- overwrite the TOC in the markdown files:
  
  Before doing so, you need to add a pair of place mark to let `md-toc` know 
  where to place the TOC. The place marks are HTML comments so they would be hidden 
  from displaying.
  ```
  <!--toc-->
  <!--toc-->
  ```
  This README.md can be the example of where to set the place marks. 
  Then using `-o` option to replace the old TOC:
  ```
  md-toc -o README1.md README2.md README3.md
  ```
  `md-toc` can batch process the markdown files. 
  Each file would be updated with the new TOC.

  If there's no `<!--toc-->` mark in the markdown file, 
  the TOC will be saved at the beginning of the file.

## Options

- `-o` aka `--overwrite`: update the markdown file with the TOC.
- `-s` aka `--show`: show the markdown file with TOC in console, no overwrite.

Without option, the `md-toc` will show only the TOC part in the console, 
not the TOC with the input file.

