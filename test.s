section .bss
	buffer resb 16384

section .text

getb:
	push ebx
	mov ecx,ebx
	mov eax,3
	mov ebx,0
	mov edx,1
	int 80h
	pop ebx
	ret

putb:
	push ebx
	mov ecx,ebx
	mov eax,4
	mov ebx,1
	mov edx,1
	int 80h
	pop ebx
	ret

global _start
_start:
	mov ebx,buffer

	mov ax,[ebx]
	mov bx,3
	div bx
	mov byte [ebx],ah
	cmp byte [ebx],0
	jz .E2

	.S2:
	add byte [ebx],2
	call putb
	dec ebx
	inc byte [ebx]
mov byte [ebx],1
	cmp byte [ebx],0
	jnz .S2

	.E2:
	cmp byte [ebx],0
	jz .E4

	.S4:
	call putb
	dec byte [ebx]
	cmp byte [ebx],0
	jnz .S4

	.E4:
	mov eax,1
	xor ebx, ebx
	int 80h