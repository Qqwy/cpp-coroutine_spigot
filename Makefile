COMMON_FLAGS = -std=c++20 -Wall -Werror -O3 -g -s -flto *.cc


all: gcc clang

clang:
	 clang++ -o spigot_clang -stdlib=libc++ $(COMMON_FLAGS)

gcc:
	g++ -o spigot_gcc  $(COMMON_FLAGS)
clean:
	rm spigot_clang
	rm spigot_gcc

format:
	find . -regex '.*\.\(cc\|hh\|impl\)' | xargs clang-format -i
