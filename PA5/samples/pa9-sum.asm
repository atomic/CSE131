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

    # $s0 == sum, $s1 == N, $t0 == i
    move $s0, $zero
    move $s1, $v0         # save user input to $s1 (N)
    li $t0, 1             # i = 0
 loop:
    beq $t0, $s1, done    # check (if i == N)
    add $s0, $s0, $t0     # sum = sum + 1
    addi $t0, $t0, 1      # i++
    b loop
 done:
    li $v0, 1              # Printing out the sum
    move $a0, $s0
    syscall

    # End Program
    li $v0, 10
    syscall
