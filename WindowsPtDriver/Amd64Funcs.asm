;   Windows Intel Processor Trace (PT) Driver 0.4
;   Filename: Amd64Funcs.asm
;	Last revision: 12/01/2016
TITLE Windows PT Driver AMD64 Assembler file

;Declare an external function
;EXTERN ExternalCFunc: PROC

.data

.code
; BOOLEAN InvokeCpuid(DWORD Index, LPVOID * lpCpuIdContext)
InvokeCpuid PROC
	MOV R8, RDX
	MOV RAX, RCX
	CPUID

	; Save the context
	TEST R8, R8
	JZ NoCtx
	MOV DWORD PTR [R8], EAX
	MOV DWORD PTR [R8+04], EAX
	MOV DWORD PTR [R8+08], EAX
	MOV DWORD PTR [R8+0Ch], EAX
	
	XOR EAX, EAX
	INC EAX

NoCtx:
	RET
InvokeCpuid ENDP

END