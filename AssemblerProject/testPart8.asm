Mov ax 5
mov dx 0
mov cx 0
fun [35] 2 0 100
put
add dx [12]
add cx 1
cmp cx ax
jb [6]
mov ax dx
put
mov [100] ax
halt











get
mov bx [34]
cmp ax [bx+1]
jb [35]
mov bx [34]
cmp ax [bx+2]
ja [35]
ret