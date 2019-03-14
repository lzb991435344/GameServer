
#include <iostream>
/**
 根据之前设计的“请求响应”和“通知”两种通信模式，需要设计出三种消息类型继承于 Message，
他们是：Request（请求包）、Response（响应包）、Notice（通知包）。

 Request 和 Response 两个类，都有记录序列号的 seq_id 字段，但 Notice 没有。Protocol 
类就是负责把一段 buffer 字节数组，转换成 Message 的子类对象。所以需要
针对三种 Message 的子类型都实现对应的 Encode() / Decode() 方法。
*/

 class Protocol 
 {
public:
	
	virtual ~Protocol(){};

	 /**
     * @brief 把请求消息编码成二进制数据
     * 编码，把msg编码到buf里面，返回写入了多长的数据，如果超过了 len，则返回-1表示错误。
     * 如果返回 0 ，表示不需要编码，框架会直接从 msg 的缓冲区读取数据发送。
     * @param buf 目标数据缓冲区
     * @param offset 目标偏移量
     * @param len 目标数据长度
     * @param msg 输入消息对象
     * @return 编码完成所用的字节数，如果 < 0 表示出错
     */
	virtual int Encode(char* buf, int offset, int len, const Request& msg) = 0;

	/**
     * 编码，把msg编码到buf里面，返回写入了多长的数据，如果超过了 len，则返回-1表示错误。
     * 如果返回 0 ，表示不需要编码，框架会直接从 msg 的缓冲区读取数据发送。
     * @param buf 目标数据缓冲区
     * @param offset 目标偏移量
     * @param len 目标数据长度
     * @param msg 输入消息对象
     * @return 编码完成所用的字节数，如果 < 0 表示出错
     */
	virtual int Encode(char* buf, int offset, int len, const Response& msg) = 0;

	 /**
     * 编码，把msg编码到buf里面，返回写入了多长的数据，如果超过了 len，则返回-1表示错误。
     * 如果返回 0 ，表示不需要编码，框架会直接从 msg 的缓冲区读取数据发送。
     * @param buf 目标数据缓冲区
     * @param offset 目标偏移量
     * @param len 目标数据长度
     * @param msg 输入消息对象
     * @return 编码完成所用的字节数，如果 < 0 表示出错
     */
	virtual int Encode(char* buf, int offset, int len, const Notice& msg);

	/**
     * 开始编码，会返回即将解码出来的消息类型，以便使用者构造合适的对象。
     * 实际操作是在进行“分包”操作。
     * @param buf 输入缓冲区
     * @param offset 输入偏移量
     * @param len 缓冲区长度
     * @param msg_type 输出参数，表示下一个消息的类型，只在返回值 > 0 的情况下有效，否则都是 TypeError
     * @return 如果返回0表示分包未完成，需要继续分包。如果返回-1表示协议包头解析出错。其他返回值表示这个消息包占用的长度。
     */
    virtual int DecodeBegin(const char* buf, int offset, int len,
                            MessageType* msg_type) = 0;

    /**
     * 解码，把之前DecodeBegin()的buf数据解码成具体消息对象。
     * @param request 输出参数，解码对象会写入此指针
     * @return 返回0表示成功，-1表示失败。
     */
    virtual int Decode(Request* request) = 0;

    /**
     * 解码，把之前DecodeBegin()的buf数据解码成具体消息对象。
     * @param request 输出参数，解码对象会写入此指针
     * @return 返回0表示成功，-1表示失败。
     */
    virtual int Decode(Response* response) = 0;

    /**
     * 解码，把之前DecodeBegin()的buf数据解码成具体消息对象。
     * @param request 输出参数，解码对象会写入此指针
     * @return 返回0表示成功，-1表示失败。
     */
    virtual int Decode(Notice* notice) = 0;
protected:
	Protocol(){};
 };

 /**
   C++ 没有内存垃圾搜集和反射的能力，在解释数据的时候，并不能一步就把一个 char[] 转换成某个子类对象，而必须分成两步处理。

先通过 DecodeBegin() 来返回，将要解码的数据是属于哪个子类型的。同时完成分包的工作，通过返回值来告知调用者，是否已经完整的收到一个包。
调用对应类型为参数的 Decode() 来具体把数据写入对应的输出变量。

对于 Protocol 的具体实现子类，我首先实现了一个 LineProtocol ，是一个非常不严谨的，基于文本ASCII编码的，用空格分隔字段，用回车分包的协议。
用来测试这个框架是否可行。因为这样可以直接通过 telnet 工具，来测试协议的编解码。然后我按照 TLV （Type Length Value）的方法设计了一个二进制的
协议。大概的定义如下：

协议分包： [消息类型:int:2] [消息长度:int:4] [消息内容:bytes:消息长度]

消息类型取值：
0x00 Error
0x01 Request
0x02 Response
0x03 Notice

包类型	字段	编码细节

Request
服务名	[字段:int:2][长度:int:2][字符串内容:chars:消息长度]
序列号	[字段:int:2][整数内容:int:4]
会话ID	[字段:int:2][整数内容:int:4]
消息体	[字段:int:2][长度:int:2][字符串内容:chars:消息长度]

Response
服务名	[字段:int:2][长度:int:2][字符串内容:chars:消息长度]
序列号	[字段:int:2][整数内容:int:4]
会话ID	[字段:int:2][整数内容:int:4]
消息体	[字段:int:2][长度:int:2][字符串内容:chars:消息长度]

Notice
服务名	[字段:int:2][长度:int:2][字符串内容:chars:消息长度]
消息体	[字段:int:2][长度:int:2][字符串内容:chars:消息长度]

一个名为 TlvProtocol 的类型完成对这个协议的实现。

*/