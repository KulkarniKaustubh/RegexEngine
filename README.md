# Regex Engine

A basic regex engine written in C as part of the course "Fine Techniques 2" (UE18CS306D) at PES University.

## Usage

```c
#include "re.h"

int main()
{
    text = <get text>
    pattern = <get pattern>
    
    /* match_arr is an array consisting [found(1)/not_found(0), start_index, end_index] */
    int *match_arr = match(pattern, text)
    
    ...
}
```

## Compile

```sh
gcc <program> re.c
```

A test program can be found in the `src` folder, named `test.c`.
