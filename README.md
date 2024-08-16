# markdown-toc-c
This filter program generates a Table of Contents (TOC) from Markdown files.
Written in C with standard libraries, making it light, fast and 
highly portable across various systems.

<!--toc-->
- [markdown-toc-c](#markdown-toc-c)
  - [Installation](#installation)
  - [Quick Start](#quick-start)
  - [Options](#options)
<!--toc-->

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
  The TOC will be printed out in th console.

- read from files:
  ```
  md-toc README.md
  ```
  The TOC will be printed out in th console.

- overwite the TOC in the markdown files:
  
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
  `md-toc` can batch process the markdown files. Each file would be updated with the new TOC.

## Options

- `-o` aka `--overwrite`: update the markdown file with the TOC.


