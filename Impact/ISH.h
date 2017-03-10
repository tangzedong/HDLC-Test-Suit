#pragma  once
public class ISocketHandler
{
public:
	virtual void onReceive() = 0;
	virtual void onAccept() = 0;
	virtual void onConnect() = 0;
	virtual void onClose() = 0;

};