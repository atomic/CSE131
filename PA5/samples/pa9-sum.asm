.data

     text:  .asciiz "Enter a number: "
     output: .asciiz "Sum output: "

.text

 main:
    # Printing out the text
    li $v0, 4
    la $a0, text
    syscall

    # Getting user input to register $t1
    li $v0, 5
    syscall
    move $t1, $v0

    # Sum starts with 0
    li $t3, 0

    # Iteartion i = t4
    li $t4, 1

# Condition Check
L0:
    slt $t5, $t1, $t4
    not $t5, $t5
    andi $t5, $t5, 1
    bne $t5, $zero, L1
    j L2
# Loop Body
L1:
    add $t3, $t3, $t4
    addi $t4, $t4, 1
    j L0
# End
L2:
    # Printing output of the sum
    li $v0, 4
    la $a0, output
    syscall

    li $v0, 1
    move $a0, $t3
    syscall
    li $v0, 10
    syscall
