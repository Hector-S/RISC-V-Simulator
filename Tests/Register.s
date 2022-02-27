# Created by: Hector Soto
# This test assembly will not follow normal conventions.
# This will test ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
# If a test passes, 1 will be stored at the current memory location.
# Otherwise a 0 will be stored.
# ADD's result will be stored at 0x0. SUB's at 0x4, and so on.
li	sp,0		# Start a memory location 0x0.
li	t0,531		# ---| ADD TEST |---
li	t1,364
li	t2,895
li	t3,0		# Set test result to 0 (false).
add	t0,t0,t1	# t0 = 531 + 364 = 895
bne	t0,t2,8		# Branch if result is incorrect.
li	t3,1		# Set test result to 1 (true).
sw	t3,0(sp)	# Store test result at current memory location.
addi	sp,sp,4		# Increment memory location.
li	t0,279		# ---| SUB TEST |---
li	t1,128
li	t2,151
li	t3,0
sub	t0,t0,t1	# t0 = 279 - 128 = 151
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,7		# ---| SLL TEST |---
li	t1,5
li	t2,224		# t2 = t1 << 5
li	t3,0
sll	t0,t0,t1	# t0 = 7 << 5
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,-47		# ---| SLT TEST |---
li	t1,-23
slt	t0,t0,t1	# slt = bool(-47 < -23)
sw	t0,0(sp)	# slt gives test result.
addi	sp,sp,4		# Increment memory location.
li	t0,452		# ---| SLTU TEST |---
li	t1,-1
sltu	t0,t0,t1	# sltu = bool(452 < 0xFFFFFFFF)
sw	t0,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,-241		# ---| XOR TEST |---
li	t1,-1		# t1 = 0xFFFFFFFF
li	t2,240
li	t3,0
xor	t0,t0,t1	# t0 = 0xFF0F XOR 0xFFFF = 0x00F0
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,72		# ---| SRL TEST |---
li	t1,3
li	t2,9
li	t3,0
srl	t0,t0,t1	# t0 = 72 >> 3
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,-3841	# ---| SRA TEST |---
li	t1,4
li	t2,-241
li	t3,0
sra	t0,t0,t1	# t0 = 0xf0ff >> 4 = 0xff0f
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,15		# ---| OR TEST |---
li	t1,64
li	t2,79
li	t3,0
or	t0,t0,t1
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,30		# ---| AND TEST |---
li	t1,-1
li	t2,30
li	t3,0
and	t0,t0,t1
bne	t0,t2,8
li	t3,1
sw	t3,0(sp)
jr	ra		# ---| END OF TESTS |---


