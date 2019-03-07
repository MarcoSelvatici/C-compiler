#### Function: f ####
## Prologue ##
f:
addiu	 $sp, $sp, -24
sw	 $ra, 20($sp)
sw	 $fp, 16($sp)
move	 $fp, $sp
sw	 $a0, 24($sp)
sw	 $a1, 28($sp)
sw	 $a2, 32($sp)
sw	 $a3, 36($sp)
## Body ##
move $t3 10
move $t4 11
addu $t1 $t3 $t4
move $t3 5
move $t4 3
divu $t3 $t4
mflo $t2
subu $t0 $t1 $t2
move	 $v0 $t0
## Epilogue ##
lw	 $a0, 24($fp)
lw	 $a1, 28($fp)
lw	 $a2, 32($fp)
lw	 $a3, 36($fp)
move	 $sp, $fp
lw	 $ra,20($sp)
lw	 $fp, 16($sp)
addiu	 $sp, $sp, 24
j	 $ra
