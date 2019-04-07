#include <iostream>

class LocalServerApp : public Application
{
public:
	LocalServerApp();
	virtual ~LocalServerApp();
	virtual int Inot(Config* cfg = NULL);
	void set_transport(Transport* transport);
	void set_protocol(Protocol* protocol);
	void set_processor(Processor* processor);
protected:
private:	
	Transport* transport_;
	Protocol* protocol_;
	Processor* processor_;
	Server* server_;
};