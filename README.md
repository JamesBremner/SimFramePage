# SimFramePage

Input of
```
RAM 10
PAGESIZE 2
NEW 1 200
NEW 2 2000
NEW 3 3000
PM
PT
END 2
PM
PT
REF 1 20
REF 3 1234
REF 3 2345
```

outputs

```
...
Page Table
Process Page    Frame

1       0       0


3       0       3
3       1       4


Process 1 referencing 20 at frame 0 offset 20
Process 3 referencing 1234 at frame 3 offset 1234
Process 3 referencing 2345 at frame 4 offset 2343
```

Note:

PT prints the page table for all processes

