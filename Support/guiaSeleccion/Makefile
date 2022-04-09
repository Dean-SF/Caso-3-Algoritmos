clean: compile
	del main.o
	del pugixml.o
compile: pugixml main
	g++ -static -std=c++17 -o main pugixml.o main.o && .\main.exe
pugixml:
	g++ -static -std=c++17 -c ../../libraries/pugixml/pugixml.cpp -o pugixml.o
main:
	g++ -static -std=c++17 -c main.cpp -o main.o