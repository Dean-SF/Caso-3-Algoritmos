Delete: Main
	del pugixml.o
	.\main.exe
Main: pugixml
	g++ -static -std=c++17 main.cpp -o main pugixml.o
pugixml:
	g++ -static -std=c++17 -c .\libraries\pugixml\pugixml.cpp -o pugixml.o