
#include <iostream>
/**
  这种强迫用户一定要实现某个接口的方式，可能会不够友好，因为针对业务逻辑设计
的类，加上一个这种接口，会比较繁琐。为了解决这种问题，我利用 C++ 的模板功能，
对于那些不想去实现 Serializable 的类型，使用一个额外的 Pack()/Upack() 模板
方法，来插入具体的序列化和反序列化方法（定义 ObjectHandlerCast 模板）。这样
除了可以减少实现类型的代码，还可以让接受消息处理的接口方法 ProcessObject()
直接获得对应的类型指针，而不是通过 Serializable 来强行转换。在这里，其实也
有另外一个思路，就是把 Serializable 设计成一个模板类，也是可以减少强制类型
转换。序列化和反序列化，以及处理业务对象，都是使用同样一个（或两个，一个输入一个输出）
模板类型参数，不如直接统一到一个类型里面好了
*/

// ObjectHandlerCast 模板定义
/**
 * 用户继承这个模板的实例化类型，可以节省关于对象序列化的编写代码。
 * 直接编写开发业务逻辑的函数。
 */

 template<typename REQ, typename RES = REQ>
class ObjectHandlerCast: public ObjectHandler
{
public:
	ObjectHandlerCast():req_obj_(NULL),res_obj_(NULL),buffer_(NULL)
	{
		buffer_ = new char[Message::MAX_MESSAGE_LENGTH];
		bzero(buffer_, Message::MAX_MESSAGE_LENGTH);
	}

	virtual ~ObjectHandlerCast()
	{
		delete[] buffer_;
	}


    /**
     * 对于不想使用 obj_ 成员来实现 Serializable 接口的，可以实现此接口
     */
	virtual int Pack(char* buffer, int length, const RES& object) const
	{
		return -1;
	}

	virtual int UnPack(const char* buffer, int length, REQ* object)
	{
		return -1;
	}
protected:
	REQ* req_obj_;
	const RES* res_obj_;
private:
	char* buffer_;

	virtual ssize_t SerializableTo(char* buffer, int buffer_length) const 
	{
		ssize_t ret = 0;
		ret = Pack(buffer, buffer_length, *res_obj_);
		return ret;
	}

	virtual int SerializableFrom(const char* buffer, int length)
	{
		req_obj_ = new REQ();
		int ret = UnPack(buffer, length, req_obj_);
		if(ret) //释放指针指向的内存
		{
			delete req_obj_;
			req_obj_ = NULL;
		}
		return ret;
	}
};

/**
  任何类型的对象，如果想要在这个框架中以网络收发，只要为他写一个模板，
完成 Pack() 和 UnPack() 这两个方法，就完成了。看起来确实方便。
（如果想节省注册的时候编写其“类名”，还需要完成一个简单的 GetName() 方法）
*/
