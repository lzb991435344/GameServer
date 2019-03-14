#include <iostream>

/**
Server 类型还需要一个 Update() 函数，让用户进程的“主循环”不停的调用，用来驱动整个
程序的运行。这个 Update() 函数的内容非常明确：

检查网络是否有数据需要处理（通过 Transport 对象）
有数据的话就进行解码处理（通过 Protocol 对象）
解码成功后进行业务逻辑的分发调用（通过 Processor 对象）

 另外，Server 还需要处理一些额外的功能，比如维护一个会话缓存池（Session），提供发送
Response 和 Notice 消息的接口。当这些工作都完成后，整套系统已经可以用来作为一个比
较“通用”的网络消息服务器框架存在了。剩下的就是添加各种 Transport/Protocol/Processor 
子类的工作。
*/

class Server
{
public:
	Server();
	virtual ~Server();

	/**
     * 初始化服务器，需要选择组装你的通信协议链
     */
	int Init(Transport* transport, Protocol* protocol, Processor* processor, Config* config = NULL);

	  /**
     * 阻塞方法，进入主循环。
     */
	void Start();

	 /**
     * 需要循环调用驱动的方法。如果返回值是0表示空闲。其他返回值表示处理过的任务数。
     */
	virtual int Update();

	//关闭当个连接，is_clear 表示是否最终整体清理
	void ClosePeer(Peer* peer, bool is_clear = false);

	 /**
     * 关闭服务器
     */
	void Close();

	/**
     * 对某个客户端发送通知消息，
     * 参数peer代表要通知的对端。
     */
	int Inform(const Notice& notice, const Peer& peer);


	/**
     * 对某个  Session ID 对应的客户端发送通知消息，返回 0 表示可以发送，其他值为发送失败。
     * 此接口能支持断线重连，只要客户端已经成功连接，并使用旧的 Session ID，同样有效。
     */
	int Inform(const Notice& notice, const std::string& session_id);


	  /**
     * 对某个客户端发来的Request发回回应消息。
     * 参数response的成员seqid必须正确填写，才能正确回应。
     * 返回0成功，其它值（-1）表示失败。
     */
	int Reply(Response* response, const Peer& peer);

	/**
     * 对某个 Session ID 对应的客户端发送回应消息。
     * 参数 response 的 seqid 成员系统会自动填写会话中记录的数值。
     * 此接口能支持断线重连，只要客户端已经成功连接，并使用旧的 Session ID，同样有效。
     * 返回0成功，其它值（-1）表示失败。
     */
	int Reply(Response* response, const std::string& session_id);

	 /**
     * 会话功能
     */
    Session* GetSession(const std::string& session_id = "", bool use_this_id = false);
    Session* GetSessionByNumId(int session_id = 0);
    bool IsExist(const std::string& session_id);
};