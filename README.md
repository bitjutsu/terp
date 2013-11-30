terp
===

A programming language because computers.

Use:
====
`>` is the terp prompt, `:` is the evaluated value of the statement.
```
> 3 + 1
: 4
> y = 4
: nil
> if y < 5 then y = 6 end
: nil
> if y == 5 then y = 4 else y = 0 end
: nil
> y
: 0
> if true then 34 else y end
: 34
```
