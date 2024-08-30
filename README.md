# Code_from_Macintosh_Programming_Secrets_1992
Code from the book "Macintosh Programming Secrets", 2nd Edition, by Scott Knaster

[![Cover art][1]][1]

  [1]: https://raw.githubusercontent.com/greenonline/Code_from_Macintosh_Programming_Secrets_1992/main/misc/Macintosh%20Prog%20Secrets.jpg "Cover art"

Chapters:
- Chapter 3 - Standard Stuff
- Chapter 4 - Dialogs
- Chapter 5 - Quick Draw
- Chapter 6 - Mouse
- Chapter 7 - Windows
- Chapter 8 - Files
- Chapter 9 - Standalone
- Chapter 10 - Various Adventures

Note: Two directories are provided, both of which contain the same source code. One (`with_LF/`) contains source files using line feeds (LF) for reading on macOS/OS X, the other (`with_CR/`) uses carriage returns (CR) for Macintosh.

## Note on `StandardStuff.c`

Each chapter's changes to `Standard_Stuff.c` are enclosed by pre-processor conditionals (`#ifdef` and `#endif`). So to activate a particular chapter's changes to `Stabdard_Stuff.c` you need to include

```none
#define __chapter<X>__
```
So, for chapter 4,
```none
#define __chapter4__
```

## Note on filenames

As all of the filenames in the varius chapters are the *same*, i.e. `Standard_Stuff.c` or `Neat_Stuff.c`, in order to distinguish the files, a suffix of `_Ch<X>` has been included, i.e. `Standard_Stuff_Ch7.c` or `Neat_Stuff_Ch4.c`

Also, instead of spaces in filenames, `Standard Stuff.c`, we use an underscore `Standard_Stuff.c`.

The resource definitions would not be parsed, when compiling `.r` files, if included with function declarations in the header file `Standard_Stuff.h`. To remedy this, a `Standard_Stuff_ResDefs.h` file was created to seperate the resource definitions from the function declarations. This file should be #included in the Standard Stuff header file and the resource defintiions commented out (or deleted) from the header file `Standard_Stuff.h`

As the resoure file `Standard_Stuff.r` is the same name for each chapter, but the contents differ wildly, a file name suffix `_Ch<X>`, has been used, i.e. `Standard_Stuff_Ch7.r`.

As the resoure file definitions `Standard_Stuff_ResDefs.r` is the same name for each chapter, but the contents differ wildly, a file name suffix `_Ch<X>`, has been used, i.e. `Standard_Stuff_ResDefs_Ch7.r`.




## See also

- [Source code for Macintosh Programming Secrets - Second Edition (1992)](https://gr33nonline.wordpress.com/2024/04/28/source-code-for-macintosh-programming-secrets-second-edition-1992/)
- [Errata - Macintosh Programming Secrets (1992)](https://gr33nonline.wordpress.com/2024/04/29/errata-macintosh-programming-secrets-1992/)

## Useful resources

- [Inside Macintosh Links](https://gr33nonline.wordpress.com/2024/04/24/inside-macintosh-links/)
- [Source code for Macintosh Programming Secrets - First Edition (1998)](https://gr33nonline.wordpress.com/2024/04/28/source-code-for-macintosh-programming-secrets-first-edition-1988/)
  
---

## Other Macintosh related repos

- greenonline/[Creation_from_Programmers_Guide_to_MPW_1990](https://github.com/greenonline/Creation_from_Programmers_Guide_to_MPW_1990)
- greenonline/[Bug_fixes_for_the_hundredrabbits_repo](https://github.com/greenonline/Bug_fixes_for_the_hundredrabbits_repo)
- greenonline/[Code_from_Macintosh_Programming_Secrets_1988](https://github.com/greenonline/Code_from_Macintosh_Programming_Secrets_1988)
- greenonline/[Code_from_Macintosh_Programming_Secrets_1992]()
- Others:
  - [hundredrabbits](https://github.com/hundredrabbits)/[Macintosh-Cookbook](https://github.com/hundredrabbits/Macintosh-Cookbook) (read only)
  - [https://git.sr.ht/~rabbits/macintosh-cookbook/tree/master](https://git.sr.ht/~rabbits/macintosh-cookbook/tree/master)
