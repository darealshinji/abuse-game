#!/usr/bin/env python

from Levenshtein import *
import sys

if len(sys.argv) < 2:
    print "Error: requires an argument (0 -- 19)"
    sys.exit(-1)

n = int(sys.argv[1])

if n < 0 or n > 19:
    print "Error: argument out of range (0 -- 19)"
    sys.exit(-1)

class Unbuffered:
    def __init__(self, stream):
        self.stream = stream
    def write(self, data):
        self.stream.write(data)
        self.stream.flush()
    def __getattr__(self, attr):
        return getattr(self.stream, attr)

sys.stdout = Unbuffered(sys.stdout)

l = [ "data-abuse/levels/level00.spe", "data-abuse/levels/level01.spe",
"data-abuse/levels/level02.spe", "data-abuse/levels/level03.spe",
"data-abuse/levels/level04.spe", "data-abuse/levels/level05.spe",
"data-abuse/levels/level06.spe", "data-abuse/levels/level07.spe",
"data-abuse/levels/level08.spe", "data-abuse/levels/level09.spe",
"data-abuse/levels/level10.spe", "data-abuse/levels/level11.spe",
"data-abuse/levels/level12.spe", "data-abuse/levels/level13.spe",
"data-abuse/levels/level14.spe", "data-abuse/levels/level15.spe",
"data-abuse/levels/level16.spe", "data-abuse/levels/level17.spe",
"data-abuse/levels/level18.spe", "data-abuse/levels/level19.spe",
"data-abuse/levels/level20.spe", "data-abuse/levels/level21.spe",
"data-frabs/levels/level00.spe", "data-frabs/levels/level01.spe",
"data-frabs/levels/level02.spe", "data-frabs/levels/level03.spe",
"data-frabs/levels/level04.spe", "data-frabs/levels/level05.spe",
"data-frabs/levels/level06.spe", "data-frabs/levels/level07.spe",
"data-frabs/levels/level08.spe", "data-frabs/levels/level09.spe",
"data-frabs/levels/level10.spe", "data-frabs/levels/level11.spe",
"data-frabs/levels/level12.spe", "data-frabs/levels/level13.spe",
"data-frabs/levels/level14.spe", "data-frabs/levels/level15.spe",
"data-frabs/levels/level17.spe", "data-frabs/levels/level18.spe",
"data-frabs/levels/level19.spe", "data-frabs/levels/level20.spe",
"data-frabs/levels/level21.spe", "data-frabs/levels/level30.spe",
"data-frabs/levels/level70.spe", "data-frabs/levels/level71.spe",
"data-frabs/levels/level72.spe", "data-frabs/levels/level73.spe",
"data-frabs/levels/level74.spe", "data-mac/levels/demo1.spe",
"data-mac/levels/demo2.spe", "data-mac/levels/demo4.spe",
"data-mac/levels/demo5.spe", "data-mac/levels/end.spe",
"data-mac/levels/flevel12.spe", "data-mac/levels/level00.spe",
"data-mac/levels/level01.spe", "data-mac/levels/level02.spe",
"data-mac/levels/level03.spe", "data-mac/levels/level04.spe",
"data-mac/levels/level05.spe", "data-mac/levels/level06.spe",
"data-mac/levels/level07.spe", "data-mac/levels/level08.spe",
"data-mac/levels/level09.spe", "data-mac/levels/level10.spe",
"data-mac/levels/level11.spe", "data-mac/levels/level12.spe",
"data-mac/levels/level13.spe", "data-mac/levels/level14.spe",
"data-mac/levels/level15.spe", "data-mac/levels/level16.spe",
"data-mac/levels/level17.spe", "data-mac/levels/level18.spe",
"data-mac/levels/level19.spe", "data-mac/levels/level20.spe",
"data-mac/levels/level21.spe", "data-mac/levels/levels.spe",
"data-mac/levels/netreg1.spe", "data-mac/levels/netshar1.spe",
"data-mac/levels/netshar2.spe" ]

c = []
for y in range(len(l)):
    for x in range(y):
        c.append((x, y))

t = len(c) / 20
for x, y in c[n * t : (n + 1) * t]:
    b = open(l[y]).read()
    a = open(l[x]).read()
    print ratio(a, b), l[x], l[y]

