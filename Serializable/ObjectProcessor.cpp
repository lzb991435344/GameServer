
#include <iostream>

class ObjectProcessor: public ProcessorHelper
{
public:
	ObjectProcessor();
	virtual ~ObjectProcessor();

  
	// 继承自 Processor 处理器函数
	virtual int Init(Server* server, Config* config = NULL);
    
    virtual int Process(const Request& request, const Peer& peer);
    virtual int Process(const Request& request, const Peer& peer, Server* server);


    ///@brief 设置默认处理器，所有没有注册具体服务名字的消息都会用这个消息处理
    inline void set_default_handler(ObjectHandler* default_handler)
    {
    	default_handler_ = default_handler;
    }

    /**
     * @brief 针对 service_name，注册对应处理的 handler ，注意 handler 本身是带对象类型信息的。
     * @param service_name 服务名字，通过 Request.service 传输
     * @param handler 请求的处理对象
     */
    void Register(const std:string& service_name, ObjectHandler* handler);


   /**
     * @brief 使用 handler 自己的 GetName() 返回值，注册服务。
     * 如果 handler->GetName() 返回 "" 字符串，则会替换默认处理器对象
     * @param handler 服务处理对象。
     */
    void Register(ObjectHandler* handler);

    ///@brief 关闭此服务
    virtual int Close();
private:
	std::map<std::string,ObjectHandler*> handler_table_;
	Config* config_;
	ObjectHandler*default_handler_;

	int ProcessBy(const Request& request, const Peer& peer,
		ObjectHandler* handler, Server* server = NULL);

	int DefaultProcess(const Request& request, const Peer& peer,
		Server* server = NULL);

	bool InitHandler(ObjectHandler* handler);
};


class ObjectHandler: public Serializable,public Updateable
{
public:
	ObjectHandler();
	virtual ~ObjectHandler();

	virtual void ProcessRequest(const Peer& peer);
	virtual void ProcessRequest(const Peer& peer, Server* server);

	/**
     * DenOS 用此方法确定进行服务注册，你应该覆盖此方法。
     * 默认名字为空，会注册为“默认服务”，就是所有找不到对应名字服务的请求，都会转发给此对象处理。
     * @return 注册此服务的名字。在 Request.service 字段中传输。
     */
	virtual std::string GetName();

	int Reply(const char* buffer, int length, const Peer& peer,
		const std::string& service_name, Server* server = NULL);

	int Inform(char* buffer, int length, const std::string& session_id,
		const std::string& service_name, Server* server = NULL);

	int Inform(const char* buffer, int length, const Peer& peer,
		const std::string& service_name, Server* server = NULL);

	virtual int Init(Server* service, Config* config);

	/**
     * 如果需要在主循环中进行操作，可以实现此方法。
     * 返回值小于 0 的话，此任务会被移除循环
     */
	virtual int Update()
	{
		return 0;
	}
private:
	Server* server_;
	std::map<int, MessageHeader> header_map_;
	Response response_;
	Notice notice_;
};
