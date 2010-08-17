.686P
.MODEL FLAT
ASSUME FS: NOTHING
.CODE

extrn _memset:PROC

;------------------------------------------------------------------------------
; Structures
;

LARGE_INTEGER   struc ; (sizeof=0x8)
LowPart         dd ?
HighPart        dd ?
LARGE_INTEGER   ends

BIT_ARRAY       struc ; (sizeof=0x8)
pbElements      dd ?
cbByteLength    dd ?
BIT_ARRAY       ends

PARSED_HET_DATA struc ; (sizeof=0x30)
field_0         dd ?
field_4         dd ?
field_8         dd ?
field_C         dd ?
field_10        dd ?
field_14        dd ?
field_18        dd ?
dwNumberOfBits  dd ?
AndMask64	    LARGE_INTEGER <>
OrMask64	    LARGE_INTEGER <>
PARSED_HET_DATA ends

BET_BLOCK_STRUCT_18 struc ; (sizeof=0x18)
pStruct04       dd ?
field_4         dd ?
field_8         dd ?
_Myfirst        dd ?
_Mylast         dd ?
_Myend          dd ?
BET_BLOCK_STRUCT_18 ends

PARSED_BET_DATA struc ; (sizeof=0xC0)
field_0         BET_BLOCK_STRUCT_18 <>
field_18        dd ?
field_1C        dd ?
field_20        dd ?
field_24        dd ?
field_28        dd ?
field_2C        dd ?
field_30        dd ?
field_34        dd ?
field_38        dd ?
field_3C        dd ?
field_40        dd ?
field_44        dd ?
field_48        dd ?
field_4C        dd ?
field_50        dd ?
field_54        dd ?
field_58        dd ?
field_5C        BET_BLOCK_STRUCT_18 <>
field_74        BET_BLOCK_STRUCT_18 <>
field_8C        BET_BLOCK_STRUCT_18 <>
field_A4        BET_BLOCK_STRUCT_18 <>
dwOpenFlags     dd ?
PARSED_BET_DATA ends

;------------------------------------------------------------------------------
; Functions
;

_aullrem        proc near               ; CODE XREF: sub_6CC140+3Ap
                                        ; sub_6CC1E0+57p ...

DividendLo      = dword ptr  8
DividendHi      = dword ptr  0Ch
DivisorLo       = dword ptr  10h
DivisorHi       = dword ptr  14h

                push    ebx
                mov     eax, [esp+DivisorHi]
                or      eax, eax
                jnz     short loc_8F8FE1
                mov     ecx, [esp+DivisorLo]
                mov     eax, [esp+DividendHi]
                xor     edx, edx
                div     ecx
                mov     eax, [esp+DividendLo]
                div     ecx
                mov     eax, edx
                xor     edx, edx
                jmp     short loc_8F9031
; ---------------------------------------------------------------------------

loc_8F8FE1:                             ; CODE XREF: _aullrem+7j
                mov     ecx, eax
                mov     ebx, [esp+DivisorLo]
                mov     edx, [esp+DividendHi]
                mov     eax, [esp+DividendLo]

loc_8F8FEF:                             ; CODE XREF: _aullrem+39j
                shr     ecx, 1
                rcr     ebx, 1
                shr     edx, 1
                rcr     eax, 1
                or      ecx, ecx
                jnz     short loc_8F8FEF
                div     ebx
                mov     ecx, eax
                mul     [esp+DivisorHi]
                xchg    eax, ecx
                mul     [esp+DivisorLo]
                add     edx, ecx
                jb      short loc_8F901A
                cmp     edx, [esp+DividendHi]
                ja      short loc_8F901A
                jb      short loc_8F9022
                cmp     eax, [esp+DividendLo]
                jbe     short loc_8F9022

loc_8F901A:                             ; CODE XREF: _aullrem+4Aj
                                        ; _aullrem+50j
                sub     eax, [esp+DivisorLo]
                sbb     edx, [esp+DivisorHi]

loc_8F9022:                             ; CODE XREF: _aullrem+52j
                                        ; _aullrem+58j
                sub     eax, [esp+DividendLo]
                sbb     edx, [esp+DividendHi]
                neg     edx
                neg     eax
                sbb     edx, 0

loc_8F9031:                             ; CODE XREF: _aullrem+1Fj
                pop     ebx
                retn    10h
_aullrem        endp

_aullshr        proc near               ; CODE XREF: sub_40E2B6+1ECp
                                        ; sub_40E2B6+240p ...
                cmp     cl, 40h
                jnb     short loc_414BDA
                cmp     cl, 20h
                jnb     short loc_414BD0
                shrd    eax, edx, cl
                shr     edx, cl
                retn
; ---------------------------------------------------------------------------

loc_414BD0:                             ; CODE XREF: _aullshr+8j
                mov     eax, edx
                xor     edx, edx
                and     cl, 1Fh
                shr     eax, cl
                retn
; ---------------------------------------------------------------------------

loc_414BDA:                             ; CODE XREF: _aullshr+3j
                xor     eax, eax
                xor     edx, edx
                retn
_aullshr        endp

; ---------------------------------------------------------------------------

BIT_ARRAY__LoadBits proc near           ; CODE XREF: sub_6D0B60+3Cp
                                        ; sub_6D0B60+54p ...

pThis           = dword ptr -10h
nByteLength     = dword ptr -0Ch
nNextByte       = dword ptr -8
pbTargetBuffer  = dword ptr -4
nBitPosition    = dword ptr  8
nBitLength      = dword ptr  0Ch
pbBuffer        = dword ptr  10h

                push    ebp
                mov     ebp, esp
                sub     esp, 10h
                mov     edx, [ebp+nBitLength]
                push    ebx
                push    esi
                push    edi
                mov     edi, ecx       ; edi = pThis
                mov     ecx, [ebp+nBitPosition]
                mov     al, cl
                mov     esi, ecx       ; ESI = nThisByte
                shr     esi, 3
                mov     ecx, edx       ; ECX = ByteLength
                shr     ecx, 3
                and     al, 7
                lea     ebx, [esi+1]
                mov     [ebp+pThis], edi
                mov     [ebp+nBitPosition], 0
                mov     [ebp+nNextByte], ebx
                test    ecx, ecx
                jbe     short loc_6CB640
                mov     edx, [ebp+pbBuffer]
                sub     edx, esi
                mov     [ebp+pbTargetBuffer], edx
                mov     [ebp+nByteLength], ecx
                mov     [ebp+nBitPosition], ecx

loc_6CB601:                             ; CODE XREF: BIT_ARRAY__LoadBits+7Bj
                test    al, al
                jnz     short loc_6CB60C
                mov     ecx, [edi+BIT_ARRAY.pbElements]
                mov     dl, [esi+ecx]
                jmp     short loc_6CB62C
; ---------------------------------------------------------------------------

loc_6CB60C:                             ; CODE XREF: BIT_ARRAY__LoadBits+43j
                mov     edi, [edi+BIT_ARRAY.pbElements]
                movzx   edx, al
                mov     ecx, 8
                sub     ecx, edx
                mov     dl, [edi+ebx]
                mov     bl, [edi+esi]
                mov     edi, [ebp+pThis] ; EDI = pThis
                shl     dl, cl
                mov     cl, al
                shr     bl, cl
                or      dl, bl
                mov     ebx, [ebp+nNextByte]

loc_6CB62C:                             ; CODE XREF: BIT_ARRAY__LoadBits+4Aj
                mov     ecx, [ebp+pbTargetBuffer]
                mov     [ecx+esi], dl
                inc     ebx
                inc     esi
                sub     [ebp+nByteLength], 1
                mov     [ebp+nNextByte], ebx
                jnz     short loc_6CB601
                mov     edx, [ebp+nBitLength]

loc_6CB640:                             ; CODE XREF: BIT_ARRAY__LoadBits+31j
                and     dl, 7
                mov     byte ptr [ebp+nBitLength+3], dl
                jz      short loc_6CB691
                mov     edx, [edi+BIT_ARRAY.pbElements]
                mov     dl, [edx+esi]
                mov     esi, [ebp+pbBuffer]
                mov     cl, al
                shr     dl, cl
                mov     ecx, [ebp+nBitPosition]
                movzx   eax, al
                mov     [ecx+esi], dl
                mov     esi, 8
                sub     esi, eax
                movzx   eax, byte ptr [ebp+nBitLength+3]
                cmp     esi, eax
                jge     short loc_6CB680
                mov     ecx, [edi]
                mov     bl, [ecx+ebx]
                mov     ecx, esi
                shl     bl, cl
                mov     ecx, [ebp+pbBuffer]
                or      bl, dl
                mov     edx, [ebp+nBitPosition]
                mov     [edx+ecx], bl

loc_6CB680:                             ; CODE XREF: BIT_ARRAY__LoadBits+AAj
                mov     ecx, eax
                mov     eax, [ebp+nBitPosition]
                mov     dl, 1
                shl     dl, cl
                mov     ecx, [ebp+pbBuffer]
                dec     dl
                and     [eax+ecx], dl

loc_6CB691:                             ; CODE XREF: BIT_ARRAY__LoadBits+86j
                pop     edi
                pop     esi
                pop     ebx
                mov     esp, ebp
                pop     ebp
                retn    10h
BIT_ARRAY__LoadBits endp

CalculateExtBlockIndex      proc near               ; CODE XREF: sub_6D82E0+44p
                                        ; sub_709F40+C7p ...

var_18          = dword ptr -18h
AndMask64       = dword ptr -10h
var_8           = dword ptr -8
var_4           = dword ptr -4
pFileValue64    = dword ptr  8
pParsedBetData  = dword ptr  0Ch

                push    ebp
                mov     ebp, esp
                sub     esp, 18h
                mov     eax, [ebp+pFileValue64]
                push    ebx
                mov     ebx, [eax+LARGE_INTEGER.HighPart]
                push    esi
                mov     esi, ecx        ; ESI = pParsedHetData
                mov     edx, [esi+PARSED_HET_DATA.AndMask64.HighPart]
                mov     ecx, [esi+PARSED_HET_DATA.AndMask64.LowPart]
                push    edi
                mov     edi, [eax+LARGE_INTEGER.LowPart] ; EBX:EDI = FileValue64
                mov     eax, [esi+PARSED_HET_DATA.field_18]
                push    0
                and     ebx, edx
                or      ebx, [esi+PARSED_HET_DATA.OrMask64.HighPart]
                and     edi, ecx
                or      edi, [esi+PARSED_HET_DATA.OrMask64.LowPart]
                push    eax
                push    ebx
                push    edi
                mov     [ebp+AndMask64], ecx
                mov     [ebp+AndMask64+04h], edx
                call    _aullrem
                mov     ecx, [esi+PARSED_HET_DATA.dwNumberOfBits]
                mov     [ebp+var_4], eax
                mov     [ebp+var_8], eax
                sub     ecx, 8
                mov     eax, edi
                mov     edx, ebx
                call    _aullshr
                mov     ecx, [ebp+AndMask64]
                mov     byte ptr [ebp+pFileValue64+3], al
                mov     eax, [ebp+AndMask64+04h]
                shrd    ecx, eax, 8
                and     ecx, edi
                mov     edi, [ebp+pParsedBetData]
                shr     eax, 8
                and     eax, ebx
                mov     [ebp+var_18], ecx
                mov     [ebp+var_18+04h], eax
                jmp     short loc_6D1000
; ---------------------------------------------------------------------------
                align 10h

loc_6D1000:                             ; CODE XREF: CalculateExtBlockIndex+68j
                                        ; CalculateExtBlockIndex+E1j
                mov     ebx, [ebp+var_4]
                mov     eax, [esi+PARSED_HET_DATA.field_C]
                mov     al, [eax+ebx]
                test    al, al
                jz      short loc_6D1073
                cmp     al, byte ptr [ebp+pFileValue64+3]
                jnz     short loc_6D1063
                mov     edx, [esi+PARSED_HET_DATA.field_0]
                mov     ecx, [esi+PARSED_HET_DATA.field_8]
                imul    edx, ebx        ; EDX = EDX * EBX
                push    4
                lea     eax, [ebp+pParsedBetData]
                push    eax
                push    ecx
                mov     ecx, [esi+PARSED_HET_DATA.field_10]
                push    edx
                mov     [ebp+pParsedBetData], 0
                call    BIT_ARRAY__LoadBits
                mov     edx, [edi+PARSED_BET_DATA.field_44]
                imul    edx, [ebp+pParsedBetData]
                mov     ecx, [edi+PARSED_BET_DATA.field_4C]
                xor     eax, eax
                push    8
                mov     [ebp+AndMask64], eax
                mov     [ebp+AndMask64+04h], eax
                lea     eax, [ebp+AndMask64]
                push    eax
                push    ecx
                mov     ecx, [edi+PARSED_BET_DATA.field_50]
                push    edx
                call    BIT_ARRAY__LoadBits
                mov     eax, [ebp+AndMask64]
                cmp     eax, [ebp+var_18]
                jnz     short loc_6D1063
                mov     ecx, [ebp+AndMask64+04h]
                cmp     ecx, [ebp+var_18+04h]
                jz      short loc_6D107F

loc_6D1063:                             ; CODE XREF: CalculateExtBlockIndex+80j
                                        ; CalculateExtBlockIndex+C9j
                xor     edx, edx
                lea     eax, [ebx+1]    ; EDX:EAX = var_4 + 1
                div     [esi+PARSED_HET_DATA.field_18]
                mov     [ebp+var_4], edx
                cmp     edx, [ebp+var_8]
                jnz     short loc_6D1000

loc_6D1073:                             ; CODE XREF: CalculateExtBlockIndex+7Bj
                pop     edi
                pop     esi
                or      eax, 0FFFFFFFFh
                pop     ebx
                mov     esp, ebp
                pop     ebp
                retn    8
; ---------------------------------------------------------------------------

loc_6D107F:                             ; CODE XREF: CalculateExtBlockIndex+D1j
                mov     eax, [ebp+pParsedBetData]
                pop     edi
                pop     esi
                pop     ebx
                mov     esp, ebp
                pop     ebp
                retn    8
CalculateExtBlockIndex      endp

_wow_CalculateExtBlockIndex  PROC
                push    ebp
                mov     ebp, esp
                
                mov     ecx, [ebp+8]
                push    [ebp+0Ch]
                push    [ebp+10h]
                call    CalculateExtBlockIndex
                
                mov     ebp, esp
                pop     ebp
                ret
_wow_CalculateExtBlockIndex  ENDP

END

