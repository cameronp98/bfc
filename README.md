bfc
===

bfc is a slightly optimising brainfuck compiler

As an example:
```brainfuck
++++++++[---].
```
would be optimised to:
```nasm
; ...
mov byte ax,8
div 3
mov byte [ebx],ah
call putb
; ...
```
