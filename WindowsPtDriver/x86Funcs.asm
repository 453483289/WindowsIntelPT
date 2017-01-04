;   Windows Intel Processor Trace (PT) Driver
;   Filename: Amd64Funcs.asm
;	Last revision: 12/01/2016
TITLE Windows PT Driver x86 Assembler file
.386
.MODEL FLAT

;Declare an external function
;EXTERN ExternalCFunc: PROC

.data
_TestAsmFunc@0 PROC
mov eax, 0c098abh
mov edx, eax
ret
_TestAsmFunc@0 ENDP

.code

END