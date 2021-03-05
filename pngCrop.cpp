#include <iostream>

#include "borderDetect.h"

using namespace std;

int main(int argc, char* argv[]) {
	/*string filename;
	if (argc > 1) {
		filename = std::string(argv[1]);
	}
	else {
		cout << "Load error\n";
		return 0;
	}*/

	string filename = ".//borders_example.png";


	size_t loc = filename.find_last_of('.');
	string dot = filename.substr(loc);
	if (dot.compare(".png") != 0)
	{
		cout << "wrong file type, must be .png\n";
		exit(EXIT_FAILURE);
	}

	// saves to project folder as 'output.png'
	borderDetect a(filename);

}