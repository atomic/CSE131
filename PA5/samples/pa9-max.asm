.data

     text:  .asciiz "Enter a number: "

.text

 main:
    # Printing out the text
    li $v0, 4
    la $a0, text
    syscall

    # Getting user input
    li $v0, 5
    syscall

    # Moving the integer input to another register
    move $t0, $v0

    # Printing out the text
    li $v0, 4
    la $a0, text
    syscall

    # Getting user input
    li $v0, 5
    syscall

    # Moving the integer input to another register
    move $t1, $v0

    slt $t2, $t0, $t1
    bne $t2, $zero, L0
    j L1

 L0:
    # Printing out the number $t1 ($t0 < $t1)
    li $v0, 1
    move $a0, $t1
    syscall
    j exit


 L1:
   # Printing out the number $t0 ($t1 < $t0)
    li $v0, 1
    move $a0, $t0
    syscall
    j exit

 exit:
    # End Program
    li $v0, 10
    syscall
