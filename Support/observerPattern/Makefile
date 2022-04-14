clean: compile
	del observerPatternAnimatorTest.o
	del pugixml.o
compile: pugixml observerPatternAnimatorTest
	g++ -static -std=c++17 -o observerPatternAnimatorTest pugixml.o observerPatternAnimatorTest.o && .\observerPatternAnimatorTest.exe
pugixml:
	g++ -static -std=c++17 -c ../../libraries/pugixml/pugixml.cpp -o pugixml.o
observerPatternAnimatorTest:
	g++ -static -std=c++17 -c observerPatternAnimatorTest.cpp -o observerPatternAnimatorTest.o