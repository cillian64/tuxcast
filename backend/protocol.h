// This is a bunch of parsers of the tuxcast protocol
#ifndef PROTOCOL_H
#define PROTOCOL_H

// Terms: A set is a combination of a command and it's parameters
// A set it not necessarily a valid command, it might not have enough parameters
// Or the wrong format.
// Line: A line contains all the same stuff as a set, but it's in a single string in the format sent over the net in/

#include <string>
#include <vector>

class set
{
	public:
		string command;
		vector<string> parameters;

		void parse(string line);
};


#endif
