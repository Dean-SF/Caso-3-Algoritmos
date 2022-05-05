clean: compile
	del AnimationTest.o
	del pugixml.o
compile: pugixml AnimationTest
	g++ -static -std=c++17 -o AnimationTest pugixml.o AnimationTest.o && .\AnimationTest.exe
pugixml:
	g++ -static -std=c++17 -c ../../libraries/pugixml/pugixml.cpp -o pugixml.o
AnimationTest:
	g++ -static -std=c++17 -c AnimationTest.cpp -o AnimationTest.o