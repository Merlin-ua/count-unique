import random

N = 1000000
M = 1000
print N
print ' '.join(map(str, [random.randrange(M) for _ in xrange(N)]))
T = 20
print T
for i in xrange(T):
	a, b = random.randrange(N), random.randrange(N)
	if a > b:
		a, b = b, a
	print a, b
