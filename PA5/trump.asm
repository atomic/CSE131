sum:
  lw $t0, 0($sp)
  lw $t1, 4($sp)
  addi $sp, $sp, -4
  add $t1, $t0, $t1
  move $v0, $t1
  addi $sp, $sp, 4
  jr $ra
main:
  addi $sp, $sp, -12
  li $t3, 5
  li $t4, 3
  # save registers...
  sw $t3, 0($sp)
  sw $t4, 4($sp)
  sw $t2, 8($sp)
  addi $sp, $sp, -4
  sw $t3, 0($sp)
  addi $sp, $sp, -4
  sw $t4, 0($sp)
  jal sum
  move $t2, $v0
  addi $sp, $sp, 8
  # restore registers...
  sw $t3, 0($sp)
  sw $t4, 4($sp)
  sw $t2, 8($sp)
  li $v0, 1
  move $a0, $t2
  syscall
  addi $sp, $sp, 12
  li $v0, 10
  syscall
