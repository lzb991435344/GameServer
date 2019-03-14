
#include <iostream>

using namespace std;

/**
	为了实现服务器之间的通信，还需要设计和 Tansport 对应的一个类型：Connector 。
这个抽象基类，用于以客户端模型对服务器发起请求。其设计和 Transport 大同小异。
除了 Linux 环境下的 Connecotr ，实现了在 C# 下的代码，以便用 Unity 开发
的客户端可以方便的使用。由于 .NET 本身就支持异步模型，所以其实现也不费太多功夫。*/

/**
 * @brief 客户端使用的连接器类，代表传输协议，如 TCP 或 UDP
 */

 class Connector
 {
 public:
 	//析构函数,也会析构子类
 	virtual ~Connector(){}

 	/**
     * @brief 初始化建立连接等
     * @param config 需要的配置
     * @return 0 为成功
     */
 	virtual int Init(Config* config) = 0;

 	/**
     * @brief 关闭
     */
 	virtual void Close() = 0;

 	 /**
     * @brief 读取是否有网络数据到来
     * 读取有无数据到来，返回值为可读事件的数量，通常为1
     * 如果为0表示没有数据可以读取。
     * 如果返回 -1 表示出现网络错误，需要关闭此连接。
     * 如果返回 -2 表示此连接成功连上对端。
     * @return 网络数据的情况
     */
    virtual int Peek() = 0;

    /**
     * @brief 读取网络数 
     * 读取连接里面的数据，返回读取到的字节数，如果返回0表示没有数据，
     * 如果buffer_length是0, 也会返回0，
     * @return 返回-1表示连接需要关闭（各种出错也返回0）
     */
    virtual int Read(char* ouput_buffer, int buffer_length) = 0;

    /**
     * @brief 把input_buffer里的数据写入网络连接，返回写入的字节数。
     * @return 如果返回-1表示写入出错，需要关闭此连接。
     */
   virtual int Write(const char* input_buffer, int buffer_length) = 0;
 protected:
 	Connector(){}
 };
