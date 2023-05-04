build: array.c case1.c case2.c func.c pointer.c
	gcc -o ./dist/array   ./array.c
	gcc -o ./dist/case1   ./case1.c
	gcc -o ./dist/case2   ./case2.c
	gcc -o ./dist/func    ./func.c
	gcc -o ./dist/pointer ./pointer.c

test:
	./dist/array
	./dist/case1
	./dist/case2
	./dist/func
	./dist/pointer

clean:
	rm -rf ./dist/*