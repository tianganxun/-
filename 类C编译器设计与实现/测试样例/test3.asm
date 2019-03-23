ASSUME CS:codesg,DS:datasg
datasg segment
    x DB '?'
    y DB '?'
    z DB '?'
    tmpB DB '?'
datasg ends
codesg segment
start:
mov AX,datasg
mov DS,AX
    mov BL,5
    mov x,BL
    mov BL,4
    mov y,BL
    mov BL,x
    add BL,y
    mov tmpB,BL
    mov BL,tmpB
    mov z,BL
mov ax,4C00H
int 21H
codesg ends
end start
