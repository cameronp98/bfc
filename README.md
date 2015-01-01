bfc
===

bfc is a slightly optimising brainfuck compiler

As an example, the brainfuck code:
```brainfuck
++++++++[---].
```
would be converted into its equivalent in NASM assembly:
```nasm
; ...
mov byte ax,8
div 3
mov byte [ebx],ah
call putb ; output one byte
; ...
```
