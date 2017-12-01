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

    # Printing out the number
    li $v0, 1
    move $a0, $t0
    syscall

    # End Program
    li $v0, 10
    syscall
