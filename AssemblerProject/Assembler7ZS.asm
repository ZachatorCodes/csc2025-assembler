; CX is counter
MOV CX 0

; DX is total sum
MOV DX CX

CMP CX 10
JAE [18]
GET
ADD DX AX
ADD CX 1
JMP [7]

PUT
HALT