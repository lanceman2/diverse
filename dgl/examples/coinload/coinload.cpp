#include <Inventor/nodes/SoSeparator.h> //Used to load the file
#include <dgl.h>    //For dgl
#include <dcoin.h>  //For dcoin
#include <iostream> //For cin/cout
using namespace std;

// ----------------------------------------------------------------------
// Example program on how to use Open Inventor content with DGL
// By Andrew A. Ray
// Usage:  ./coinfly modelname

int main(int argc, char** argv)
{
	//Check to see if the program is called properly
	if (argc !=2)
	{
		cout << "Invalid usage of " << argv[0] << endl;
		cout << "Usage:  " << argv[0] << " filetoload\n";
		return -1;
	}
	//Start DGL
	dglInit();
	//Start the coin interface to DGL
	DCoin::init();

	//By default Coin flips the world to OpenGL coordinates
	//Uncommenting this will cause the world to be in DIVERSE coordinates
	//DCoin::flipWorld(false);

	//Load the model
	SoSeparator* Temp = DCoin::loadFile(argv[1]);
	if (Temp == NULL)
	{
		cout << "Could not load file: " << argv[1] << endl;
		return 0;
	}
	//Add the content to the world
	DCoin::getWorld()->addChild(Temp);
	//Start DGL
	dglStart();
	//Cause the main loop to execute
	dglRun();
}
