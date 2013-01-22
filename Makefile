CC=g++

main: unique.cc
		$(CC) unique.cc -o unique

clean:
		rm -f unique
		rm -f large_input.txt

gentest: generate_input.py
		python generate_input.py > large_input.txt

test: main gentest
		cat large_input.txt | ./unique
