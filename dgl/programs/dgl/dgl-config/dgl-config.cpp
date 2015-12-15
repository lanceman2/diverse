#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#include <dgl/config.h>
void Usage()
{
	printf(
	       "USAGE:  dgl-config [--about | --cflags | --compiler | --include\n"
	       "        | --coin-include | --osg--include | --vtk-include | --libs\n"
	       "        | --coin-libs |  --osg-libs | --vtk-libs | --test --version]\n\n"
	       "dgl-config is an information report tool to get information about \n"
	       "the DIVERSE graphics interface to Open GL(DGL)\n"
	       "dgl-config helps you compile and link your code against a DGL installation\n\n"
	       "--about\t\tlists information about DGL\n"
	       "--cflags\tprint compiler (-I) include paths and related \n\t\tcompiler options\n"
	       "--compiler\tprint the name of the compiler that was used to compile DGL\n"
	       "--data-dir\tprint the directory where the DGL data files are stored\n"
	       "--dso-dir\tprint the directory where the DGL DSO's are stored\n"
	       "--include\tprint compiler -I include path options\n"
	       "--coin-include\tprint compiler -I include path options for Coin\n"
	       "--osg-include\tprint compiler -I include path options for Open Scene Graph\n"
	       "--vtk-include\tprint compiler -I include path options for VTK\n"
	       "--libs\t\tprint necessary link functions (-L, -ldgl, etc...)\n"
	       "--coin-libs\tprint necessary link functions for Coin (-L, -ldcoin, etc...)\n"
	       "--osg-libs\tprint necessary link functions for Open Scene Graph(-L, -ldosg, etc...)\n"
	       "--vtk-libs\tprint necessary link functions for VTK (-L, -ldvtk, etc...)\n"
	       "--root\t\tprint the root directory of the DGL installation\n"
	       "--test\t\treturns 0, lets you know `dgl-config` can run\n"
	       "--version\tprints the DGL verision\n") ;
}
int handleArg(char* arg);
int main(int argc, char** argv)
{
	if (argc !=2)
	{
		Usage();
		return 0;
	}
	return handleArg(argv[1]);
}

int handleArg(char*  arg)
{
	if (!strcmp(arg, "--test"))
	{
		return 0;	
	}
	else if (!strcmp(arg, "--cflags"))
	{
		printf("%s\n", DGL_CXX_FLAGS);
	}
	else if (!strcmp(arg, "--compiler"))
	{
		printf("%s\n", DGL_COMPILER);
	}
	else if (!strcmp(arg, "--include"))
	{
		printf("%s\n", DGL_INCLUDE);
	}
	else if (!strcmp(arg, "--version"))
	{
		printf("%s\n", DGL_VERSION);
	}
	else if(!strcmp(arg, "--about"))
	{
		printf(
		       "DGL - Diverse Interface to OpenGL, version %s\n"
		       "by Andrew A. Ray, original idea from Chris Logie, with\n"
		       "funding, encouragement and vision by Ron Kriz.\n"
		       "OSG programs and DSOs by John Kelso, with\n"
		       "help from Andrew, NIST, and many others.\n\n"
		       "Compiled on %s for architecture type %s\n\n"
		       "Visit the DIVERSE homepage, %s\n"
		       ,DGL_VERSION, DGL_COMPILE_DATE, DGL_ARCH, DGL_HOMEPAGE
		       );
	}
	else if(!strcmp(arg, "--libs"))
	{
		printf("%s\n", DGL_LIBS);
	}
	else if(!strcmp(arg, "--osg-include"))
	{
		printf("%s\n", DOSG_INCLUDE);
	}
	else if(!strcmp(arg, "--osg-libs"))
	{
		printf("%s\n", DOSG_LIBS);
	}
	else if(!strcmp(arg, "--coin-libs"))
	{
		printf("%s\n", DCOIN_LIBS);
	}
	else if(!strcmp(arg, "--coin-include"))
	{
		printf("%s\n", DCOIN_INCLUDE);
	}
	else if(!strcmp(arg, "--vtk-include"))
	{
		printf("%s\n", DVTK_INCLUDE);
	}
	else if(!strcmp(arg, "--vtk-libs"))
	{
		printf("%s\n", DVTK_LIBS);
	}
	else if(!strcmp(arg, "--root"))
	{
		printf("%s\n", DGL_DEFAULT_ROOT);
	}
	else if(!strcmp(arg, "--dso-dir"))
	{
		printf("%s\n", DGL_DSO_DIR);
	}
	else if(!strcmp(arg, "--data-dir"))
	{
		printf("%s\n", DGL_DATA_DIR);
	}
	else
	{
		Usage();
	}
	return 0;
}
