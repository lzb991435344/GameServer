#include <iostream>

 /**
	 处理器层是我设计用来对接具体业务逻辑的抽象层，它主要通过输入参数 Request 
	和 Peer 来获得客户端的输入数据，然后通过 Server 类的 Reply()/Inform() 
	来返回 Response 和 Notice 消息。实际上 Transport 和 Protocol 的子类们，
	都属于 net 模块，而各种 Processor 和 Server/Client 这些功能类型，属于另
	外一个 processor 模块。这样设计的原因，是希望所有 processor 模块的代码单
	向的依赖 net 模块的代码，但反过来不成立。

 */

 ///@brief 处理器基类，提供业务逻辑回调接口

class Processor
{
public:
	Processor();
	virtual ~Processor();

	/**
     * 初始化一个处理器，参数server为业务逻辑提供了基本的能力接口。
     */
	virtual int Init(Server* server, Config* config = NULL);

	 /**
     * 处理请求-响应类型包实现此方法，返回值是0表示成功，否则会被记录在错误日志中。
     * 参数peer表示发来请求的对端情况。其中 Server 对象的指针，可以用来调用 Reply(),
     * Inform() 等方法。如果是监听多个服务器，server 参数则会是不同的对象。
     */
	virtual int Process(const Request& request, const Peer& peer, Server* server);

	/**
     * 关闭清理处理器所占用的资源
     */
	virtual int Close();
};

/**
 设计完 Transport/Protocol/Processor 三个通信处理层次后，就需要一个组合这三个层次的代码，
那就是 Server 类。这个类在 Init() 的时候，需要上面三个类型的子类作为参数，以组合成不同功能
的服务器


TlvProtocol tlv_protocol;   //  Type Length Value 格式分包协议，需要和客户端一致
TcpTransport tcp_transport; // 使用 TCP 的通信协议，默认监听 0.0.0.0:6666
EchoProcessor echo_processor;   // 业务逻辑处理器
Server server;  // DenOS 的网络服务器主对象
// 组装一个游戏服务器 对象：TLV 编码、TCP 通信和回音服务
server.Init(&tcp_transport, &tlv_protocol, &echo_processor);  */


