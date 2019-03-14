#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>


/**
服务器传输层在异步模型下的基本使用序列：

1.在主循环中，不断尝试读取有什么数据可读
2.如果上一步返回有数据到达了，则读取数据
3.读取数据处理后，需要发送数据，则向网络写入数据
*/

class Transport
{
public:
	/**
    * 初始化Transport对象，输入Config对象配置最大连接数等参数，可以是一个新建的Config对象。
    */  
	virtual int Init(Config* config) = 0;

	 /**
    * 检查是否有数据可以读取，返回可读的事件数。后续代码应该根据此返回值循环调用Read()提取数据。
    * 参数fds用于返回出现事件的所有fd列表，len表示这个列表的最大长度。如果可用事件大于这个数字，并不影响后续可以Read()的次数。
    * fds的内容，如果出现负数，表示有一个新的终端等待接入。
    */
	virtual int Peek(int* fds, int len) = 0;

	/**
    * 读取网络管道中的数据。数据放在输出参数 peer 的缓冲区中。
    * @param peer 参数是产生事件的通信对端对象。
    * @return 返回值为可读数据的长度，如果是 0 表示没有数据可以读，返回 -1 表示连接需要被关闭。
    */
	virtual int Read(Peer* peer) = 0;

	/**
    * 写入数据，output_buf, buf_len为想要写入的数据缓冲区及长度，output_peer为目标队端，
    * 返回值表示成功写入了的数据长度。-1表示写入出错。
    */
	virtual int Write(const char* output_buf, int buf_len, const Peer& output_peer) = 0;

	/**
    * 关闭一个对端的连接
    */
	virtual void ClosePeer(const Peer& peer) = 0;

	 /**
    * 关闭Transport对象。
    */
    virtual void Close();
};

/**
	需要有一个 Peer 类型。这个类型是为了代表通信的客户端（对端）对象。在一般的 Linux 系统中，
一般我们用 fd （File Description）来代表。但是因为在框架中，我们还需要为每个客户端建立
收数据的缓存区，以及记录通信地址等功能，所以在 fd 的基础上封装了一个这样的类型。这样也有
于把 UDP 通信以不同客户端的模型，进行封装。

	游戏使用 UDP 协议的特点：一般来说 UDP 是无连接的，但是对于游戏来说，是肯定需要有明确的客户端的，所以就不能
简单用一个 UDP socket 的fd 来代表客户端，这就造成了上层的代码无法简单在 UDP 和 TCP 之间保持一致。因此这里
用 Peer 这个抽象层，正好可以解决这个问题。这也可以用于那些使用某种消息队列中间件的情况，因为可能这些中间件，
也是多路复用一个fd 的，甚至可能就不是通过使用 fd 的 API 来开发的。

	对于上面的 Transport 定义，对于 TCP 的实现者来说，是非常容易能完成的。但是对于 UDP 的实现者来说，则需要考
虑如何充分利用 Peer ，特别是 Peer.fd_ 这个数据。我在实现的时候，使用了一套虚拟的 fd 机制，通过一个客户端
的 IPv4 地址到 int 的对应 Map ，来对上层提供区分客户端的功能。在 Linux 上，这些 IO 都可以使用 epoll 库
来实现，在 Peek() 函数中读取 IO 事件，在 Read()/Write() 填上 socket 的调用就可以了。
*/

///@brief 此类型负责存放连接过来的客户端信息和数据缓冲区
class Peer
{
public:
	int buf_size_;//缓冲区的长度
	char* const buffer_;//缓冲区的起始地址，不能改变指向
	int produce_pos_;//填入数据的长度
	int consumed_pos_;//消耗了数据的长度

	int GetFd() const;
	void SetFd(int fd);//获取本地地址
	const struct sockaddr_in& GetLocalAddr() const;
	void SetLocalAddr(const struct sockaddr_in& localAddr);

	//获得远程的地址
	const struct sockaddr_in& GetRemoteAddr() const;
	void SetRemoteAddr(const struct sockaddr_in& remoteAddr);
private:
	int fd_;//收发数据使用的fd
	struct sockaddr_in remote_addr_;//对端地址
	struct sockaddr_in local_addr_;//本端地址
};