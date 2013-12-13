terp
===

A programming language because computers.

Use:
====
`>` is the terp prompt, `:` is the evaluated value of the statement.
```
> 3 + 1
: 4
> x = y = 4
: 4
> x
: 4
> if y < 5 then y = 6 end
: 6
> if y == 5 then y = 4 else y = 0 end
: 0
> y
: 0
> if true then 34 else y end
: 34
```
