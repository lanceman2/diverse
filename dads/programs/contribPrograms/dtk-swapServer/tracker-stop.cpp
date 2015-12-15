#include <dtk.h>

int main()
{
	dtkSharedMem STOP(sizeof(int), "tracker-server");
	int VAL=1337;
	STOP.write(&VAL);
	return 0;
}
