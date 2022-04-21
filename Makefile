Main: pugixml
	g++ -static -std=c++17 main.cpp -o main pugixml.o && .\main.exe
pugixml:
	g++ -static -std=c++17 -c .\libraries\pugixml\pugixml.cpp -o pugixml.o