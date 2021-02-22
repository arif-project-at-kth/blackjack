  # labwork.S
  # Written 2015 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1 2015-2016.
  # The macros PUSH and POP are in the public domain.

.global delay  
.global	time2string 
.global enable_interrupt

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

  # You can add your own code here
  #
  
hexasc:
	andi	$t0, $a0, 0x0F	# t0 = a0 AND 1111
	
	addi	$v0, $t0, 0x30	# v0 = t0 + 48
	
	slti	$t1, $v0, 0x3A	# t1 = v0 < 58
	bne	$t1, $zero, return	# v0 == 0 ? return : contiune
	
	addi	$v0, $v0, 0x07	# v0 = v0 + 7

return:
	jr	$ra
	nop	

delay:
	PUSH	$ra
	
	li	$t0, 1000
	li	$t2, 9000000
	
	slt	$t0, $t0, $zero
	bne	$t0, $zero, while
	nop
	
	POP	$ra
	jr	$ra
	nop
	
while:
	addi	$t0, $t0, -1
	slt		$t0, $t0, $zero
	beq		$t0, $zero, done
	nop
	add	$t1, $zero, $zero
forloop:	# 4711 = 0x1267, # 5000 = 0x1388, 4400 = 0x1130 (sec)
	beq		$t1, $t2, while
	addi	$t1, $t1, 1 
	j		forloop 

done:
	j	delay
	nop

	
	
time2string:
	PUSH $s1
	PUSH $ra
	move $s1,$a0
	

	andi $t0, $a1, 0xf000
	srl $a0,$t0,12
	jal hexasc
	nop
	move $a2,$v0
	sb $a2,0($s1)
	
	andi $t0, $a1, 0x0f00
	srl $a0,$t0,8
	jal hexasc
	nop
	move $a2,$v0
	sb $a2,1($s1)
	
	li $t0, 0x3a
	sb $t0, 2($s1)
	
	andi $t0, $a1, 0x00f0
	srl $a0,$t0,4
	jal hexasc
	nop
	move $a2,$v0
	sb $a2,3($s1)
	
	andi $t0, $a1, 0x000f
	srl $a0,$t0,0
	jal hexasc
	nop
	move $a2,$v0
	sb $a2,4($s1)
	
	li $t0, 0x0
	sb $t0,5($s1)
	
	POP $ra
	POP $s1
	
	jr $ra
	nop
	
	nop
	
 enable_interrupt:
 	ei
 	jr $ra
 	nop
