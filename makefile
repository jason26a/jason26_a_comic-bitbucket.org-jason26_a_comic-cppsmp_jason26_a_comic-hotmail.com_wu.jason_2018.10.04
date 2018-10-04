exe=main
RM=rm -f

all: main child1 child2
	# -I/opt/wandbox/boost-1.68.0/gcc-head/include
	
main:
	g++ -std=c++11 -pthread main.cpp -o main.out -I /jason/boost_1_65_0 -lboost_filesystem -lboost_system 
	
child1:
	g++ -std=c++11 -pthread child1.cpp -o child1.out -I /jason/boost_1_65_0 -lboost_system -lrt
	
child2:
	g++ -std=c++11 -pthread child2.cpp -o child2.out -I /jason/boost_1_65_0 -lboost_system -lrt
	
clean:
	rm -f main.out child1.out child2.out
