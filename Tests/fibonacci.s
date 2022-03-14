# Created by: Hector Soto
# This test assembly will not follow normal conventions.
# This will store the first 15 Fibonacci numbers in memory.

li	t3,14		# 13 iterations. 15 Fibonacci numbers
li	t4,1		# Stop when 1 is reached in loop.
li	t0,0
sw	t0,0(sp)	# Store 0 first memory location
addi	sp,sp,4
li	t1,1
sw	t1,0(sp)	# Store 1 at second memory location
lw	t0,-4(sp)	# Start of loop.
lw	t1,0(sp)
add	t1,t1,t0	# Add last two fibonacci numbers.
addi	sp,sp,4		# Increment pointer.
sw	t1,0(sp)	# Store newest fibonacci number.
addi	t3,t3,-1	# Decrement loop counter.
beq	t3,t4,8		# Skip jump to start of loop if not done.
jal	zero,-20
jr	ra		# End program
