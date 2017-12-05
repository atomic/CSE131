.data
.text
main:
  addi $sp, $sp, -12
  li $t0, 5
  li $t1, 3
  # save registers...
  sw $t1, 0($sp)
  sw $t0, 4($sp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  jal sum
  move $t2, $v0
  # restore registers...
  lw $t1, 0($sp)
  lw $t0, 4($sp)
  addi $sp, $sp, 8
  li $v0, 1
  move $a0, $t2
  syscall
  addi $sp, $sp, 12
  # End Program
  li $v0, 10
  syscall
  sum:
  lw $t3, 0 ($sp)
  lw $t4, 4 ($sp)
  addi $sp, $sp, -4
  add $t5, $t3, $t4
  move $v0, $t5
  addi $sp, $sp, 4
  jr $ra
  # End program
  li $v0, 10
  syscall
