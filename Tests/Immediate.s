# Created by: Hector Soto
# This test assembly will not follow normal conventions.
# This will test ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
# If a test passes, 1 will be stored at the current memory location.
# Otherwise a 0 will be stored.
# ADDI's result will be stored at 0x0. SLTI's at 0x4, and so on.
li	sp,0		# Start at memory location 0x0.
li	t0,5		# ---| ADDI TEST |---
li	t1,8		# Expected ADDI result.
li	t3,0		# Set test result to 0 (false).
addi	t0,t0,3		# ADDI 5 + 3. Result should be 8.
bne	t0,t1,8		# If ADDI 5 + 3 != 8, ADDI failed. Jump past success.
li	t3,1		# Set test result to 1 (true).
sw	t3,0(sp)	# Store test result at current memory location.
addi	sp,sp,4		# Increment to next memory location to store next test result.
li	t0,-83		# ---| SLTI TEST |---
slti	t0,t0,-47	# t0 = 1 if -83 < -47.
sw	t0,0(sp)	# SLTI gives test result. No branch code needed.
addi	sp,sp,4		# Increment memory location.
li	t0,457		# ---| SLTIU TEST |---
sltiu	t0,t0,-1	# (unsigned)-1 should be > 457.
sw	t0,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t1,0		# ---| XORI TEST |---
li	t3,0		# Set test result to 0.
li	t0,-1		# Load 0xFFFFFFFF.
xori	t0,t0,-1	# Result should be 0.
bne	t0,t1,8
li	t3,1		# Set test result to 1.
sw	t3,0(sp)	# Store test result
addi	sp,sp,4		# Increment memory location.
li	t0,0		# ---| ORI TEST |---
li	t1,-1384
li	t3,0
ori	t0,t0,-1384	# t0 = -1384
bne	t0,t1,8
li	t3,1		# Set test result to 1.
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,-1		# ---| ANDI TEST |---
li	t1,240		# Set t1 = 0x0F0.
li	t3,0
andi	t0,t0,240	# Set t0 = 0x0F0.
bne	t0,t1,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,212		# ---| SLLI TEST |---
li	t1,848		# t1 = t0 << 2
li	t3,0		# Set test result to 0.
slli	t0,t0,2		# Shift left twice.
bne	t0,t1,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,480		# ---| SRLI TEST |---
li	t1,15		# t1 = t0 >> 5
li	t3,0
srli	t0,t0,5		# Shift right 5 times.
bne	t0,t1,8
li	t3,1
sw	t3,0(sp)
addi	sp,sp,4		# Increment memory location.
li	t0,-241		# ---| SRAI TEST |---
li	t1,-16		# t1 = t0 >> 4
li	t3,0
srai	t0,t0,4
bne	t0,t1,8
li	t3,1
sw	t3,0(sp)	# ---| END OF TESTS |---
jr	ra
