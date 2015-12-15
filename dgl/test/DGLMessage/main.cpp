#include <dgl.h>

int main()
{
	//Class for sending the message 
	DGLPostOffice msgSender("TEST_SEGMENT");

	//Message creation
	DGLMessage msg;
	msg.setSubject("Testing-Message");
	msg.addInt(3);
	msg.addFloat(4.5);

	//Send the message
	msgSender.sendMessage(&msg);

	//Get the message back
	DGLMessage* outputMsg = NULL;
	while (outputMsg == NULL)
		outputMsg = msgSender.getMessage();

	//Output it
	printf("Message subject is %s\n", outputMsg->getSubject().c_str());
	printf("Integer value is %d\n", outputMsg->getInt());
	printf("Float value is %f\n", outputMsg->getFloat());
}
