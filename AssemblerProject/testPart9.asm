Mov ax 10
mov dx 0
mov cx dx
fun [35] 3 0 100 [101]

put
add dx [101]
add cx 1
cmp cx ax
jb [5]
mov [102] dx
mov ax dx
put
halt









get
cmp bx [34]
mov cx [bx+1]
mov dx [bx+2]
mov [bx] [bx+3]
mov ax [bx]
cmp ax cx
jb [35]
cmp ax dx
ja [35]
mov [bx] ax
ret