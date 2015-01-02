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

	mov byte [ebx],8
	.S1:
	dec ebx
	add byte [ebx],4
	.S2:
	dec ebx
	add byte [ebx],2
	dec ebx
	add byte [ebx],3
	dec ebx
	add byte [ebx],3
	dec ebx
	inc byte [ebx]
	dec byte [ebx]
	cmp byte [ebx],0
	jnz .S2

	.E2:
	dec ebx
	inc byte [ebx]
	dec ebx
	inc byte [ebx]
	dec ebx
	dec byte [ebx]
	sub ebx,2
	inc byte [ebx]
	.S3:
	cmp byte [ebx],0
	jnz .S3

	.E3:
	dec byte [ebx]
	cmp byte [ebx],0
	jnz .S1

	.E1:
	sub ebx,2
	call putb
	dec ebx
	sub byte [ebx],3
	call putb
	add byte [ebx],7
	call putb
	add byte [ebx],3
	call putb
	sub ebx,2
	call putb
	dec byte [ebx]
	call putb
	call putb
	add byte [ebx],3
	call putb
	sub byte [ebx],6
	call putb
	sub byte [ebx],8
	call putb
	sub ebx,2
	inc byte [ebx]
	call putb
	dec ebx
	add byte [ebx],2
	call putb
	mov eax,1
	xor ebx, ebx
	int 80h
