# SimFramePage

Input of
```
RAM 10
PAGESIZE 2
NEW 1 200
NEW 2 2000
PM
PT
```

outputs

```
NEW 1 200
NEW 2 2000

Frames
Frame   Process
0       1
1       2
2       2
3       free
4       free


Page Table
Process Page    Frame

1       0       0

2       0       1
2       1       2
```

Note:

PT prints the page table for all processes

REF is not yet implemented

END is not yet implemented
