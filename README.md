# bfc - a *slightly optimising* brainfuck compiler

As an example, the following brainfuck code:
```brainfuck
++++++++[---].
```
would be converted by *bfc* into its equivalent in NASM assembly:
```nasm
.section .bss:
	buffer resb 32768 ; 32k memory cells

section .text:
global _start
_start:
	mov ebx,buffer

	mov al,8
	mov dl,3
	div dl
	mov byte [ebx],ah
	call putb

	; return 0
	mov eax,1
	xor ebx, ebx
	int 80h
```

---

installation (linux only):
```bash
make && sudo make install
```

usage:
```bash
# compile 'examples/hello_world.bf' with a buffer size of 30000 bytes
bfc examples/hello_world.bf -b 30000
```
