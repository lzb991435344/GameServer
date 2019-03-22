#include <iostream>

class Serializable
{
public:

/**
     * 序列化到一个数组中
     * @param buffer 目地缓冲区数组
     * @param buffer_length 缓冲区长度
     * @return 返回写入了 buffer 的数据长度。如果返回 -1 表示出错，比如 buffer_length 不够。
     */
	virtual size_t SerializableTo(char* buffer, int buffer_length) = 0;

	/**
     * @brief 从一个 buffer 中读取 length 个字节，反序列化到本对象。
     *@return  返回 0 表示成功，其他值表示出错。
     */
	virtual int SerializableFrom(const char* buffer, int length) = 0;

	virtual ~Serializable() {}
};
