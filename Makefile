all: gcc clang

clang:
	 clang++ -std=c++20 -o spigot_clang -Wall -Werror -stdlib=libc++ -O2 -s -g main.cc

gcc:
	g++ -std=c++20 -o spigot_gcc -Wall -Werror -O2 -s -g main.cc

clean:
	rm spigot_clang
	rm spigot_gcc

