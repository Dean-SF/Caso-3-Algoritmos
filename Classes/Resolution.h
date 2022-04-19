#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <sstream>
#include <string>

using std::stringstream;
using std::string;

class Resolution {
private:
    int width;
    int height;

public:

    Resolution() {
        width = 0;
        height = 0;
    }

    Resolution(int pWidth, int pHeight) {
        width = pWidth;
        height = pHeight;
    }
    
    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    void setWidth(int pWidth) {
        width = pWidth;
    }

    void setHeight(int pHeight) {
        height = pHeight;
    }
    
    void setViewBoxResolution(string pViewBox, bool offset) {
        stringstream stringManipulator(pViewBox);
        string extractedString;

        stringManipulator >> extractedString; // min-x
        stringManipulator >> extractedString; // min-y

        stringManipulator >> extractedString;
        width = stoi(extractedString);
        stringManipulator >> extractedString;
        height = stoi(extractedString);
        if(offset) {
            int offset = (width + height)/40;
            width -= offset;
            height -= offset;
        }
    }
};



#endif