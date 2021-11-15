all: gcc clang

clang:
	 clang++ -std=c++20 -o spigot_clang -Wall -Werror -stdlib=libc++ -O2 -g -s -flto *.cc

gcc:
	g++ -std=c++20 -o spigot_gcc -Wall -Werror -O2 -g -s -flto *.cc

clean:
	rm spigot_clang
	rm spigot_gcc

format:
	find . -regex '.*\.\(cc\|hh\|impl\)' | xargs clang-format -i
