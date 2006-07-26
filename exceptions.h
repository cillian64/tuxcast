#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>

using namespace std;

class eException
{
        public:
                virtual void print(void)
	                { cout << "Vanilla Exception (This shouldn't happen)" << endl; }
};

// Each separate module's exception header will include this
// And all it's exceptions derive from eException
// They are named like eModule_ExceptionName
// And all just replace the print function with more info

#endif
