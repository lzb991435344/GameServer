#include <iostream>


 /**
   消息本身被抽象成一个叫 Message 的类型，它拥有“服务名字”“会话ID”两个消息头字段，
 用以完成“分发”和“会话保持”功能。而消息体则被放在一个字节数组中，并记录下字节数组
 的长度。*/

 enum MeaasgeType
 {
 	TypeError,//错误协议
 	TypeRequest,//请求类型，客户端发往服务器
 	TypeResponse,//响应类型，服务器收到请求后返回
 	TypeNotice //通知类型，服务器主动通知客户端
 }


///@brief 通信消息体的基类
///基本上是一个 char[] 缓冲区

 struct Message
 {
 public:
 	static int MAX_MESSAGE_LENGTH;
 	static int MAX_HEADER_LENGTH;

 	MeaasgeType type; ///< 此消息体的类型(MessageType)信息
 	virtual ~Message();

 	//重载赋值运算符
 	virtual Message& operator=(const Message& right);

 	/**
     * @brief 把数据拷贝进此包体缓冲区
     */
 	void SetData(const char* input_ptr, int input_length);

 	 ///@brief 获得数据指针
 	inline char* GetData() const
 	{
 		return data_;
 	}

 	 ///@brief 获得数据长度
    inline int GetDataLen() const{
        return data_len_;
    }
    char* GetHeader() const;
    int GetHeaderLen() const;
 protected:
 	Message();
 	Message(const Message& message);
 private:
 	char* data_;//包体内容缓冲区
 	int data_len_; //包体的长度
 };