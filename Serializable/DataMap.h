#include <iostream>

/**
	简单的数据缓存使用模型
*/

class DataMap : public Updateable
{
public:
	DataMap();
	virtual ~DataMap();

	/**
     * @brief 对于可能阻塞的异步操作，需要调用这个接口来驱动回调。
     */
	virtual int Update()
	{
		return 0;
	}

	/**
     * @brief 获取 key 对应的数据。
     * @param key 数据的 Key
     * @param value_buf 是输出缓冲区指针
     * @param value_buf_len 是缓冲区最大长度
     * @return 返回 -1 表示找不到这个 key，返回 -2 表示 value_buf_len 太小，不足以读出数据。其他负数表示错误，返回 >= 0 的值表示 value 的长度
     */
	virtual int Get(const std::string& key, char* value_buf, int value_buf_len) = 0;
	virtual int Get(const std::string& key, Serializable* value);


   /**
     * @brief 异步 Get 的接口
     * @param key 获取数据的 Key
     * @param callback 获取数据的回调对象，如果 key 不存在， FetchData() 参数 value_buf 会为 NULL
     * @return 返回 0 表示发起查询成功，其他值表示错误。
     */
	virtual int Get(const std::string& key, DataMapCallback* callback) = 0;


   /**
     * @brief 覆盖、写入key对应的缓冲数据。
     * @return 成功返回0。 返回 -1 表示value数据太大，其他负数表示其他错误
     */
    
	virtualintPut(conststd::string& key, const char* value_buf, int value_buf_len) = 0;    
	virtualintPut(const std::string& key,const Serializable& value);

    
   /**
     * 写入数据的异步接口，使用 callback 来通知写入结果
     * @param key 数据 key
     * @param value 数据 Value
     * @param callback 写入结果会调用此对象的 PutResult() 方法
     * @return 返回 0 表示准备操作成功
     */
    
	virtual int Put(const std::string& key, const Serializable& value, DataMapCallback* callback);
     
	virtual int Put(const std::string& key,const char* value_buf, int value_buf_len, DataMapCallback* callback);

   /**
    * 删除 key 对应的数据
    * @return 返回 0 表示成功删除，返回 -1 表示这个 key 本身就不存在，其他负数返回值表示其他错误。
    */
	virtual int Remove(const std::string& key) = 0;

	virtual int Remove(const std::string& key, DataMapCallback* callback);

  
   /**
     * 是否有 Key 对应的数据
     *@return  返回 key 值表示找到，0 表示找不到
    */
	virtual int ContainsKey(const std::string& key) = 0;

 
   /**
     * 异步 ContainsKey 接口，如果 key 不存在， FetchData() 参数 value_buf 会为 NULL。
     * 如果 key 存在，value_buf 则不为NULL，但也不保证指向任何可用数据。可能是目标数值，
     * 也可能内部的某个空缓冲区。如果是在本地的数据，就会是目标数据，如果是远程的数据，
     * 为了减少性能就不会传入具体的 value 数值。
     */

	virtual int ContainsKey(const std::string& key, DataMapCallback* callback) = 0;


   /**
    * 遍历整个缓存。
    * @param callback 每条记录都会调用 callback 对象的 FetchData() 方法
    * @return 返回 0 表示成功，其他表示错误。
    */

	virtual int GetAll(DataMapCallback* callback) = 0;


   /**
     * 获取整个缓存的数据
     * @param result 结果会放在这个 map 里面，记得每条记录中的 Bytes 的 buffer_ptr 需要 delete[]
     * @return 返回 0 表示成功，其他表示错误
     */
	virtual int GetAll(std::map<std::string, Bytes>* result);
private:
	char* tmp_buffer_;
};

/**
std::map 的简单模仿，把 key 固定成 string ，而把 value 固定成一个 buffer 或者是一个可序列化对象。
另外为了实现分布式的缓冲，所有的接口都增加了回调接口。

可以用来充当数据缓存的业界方案其实非常多，他们包括：
(1)堆内存，这个是最简单的缓存容器
(2)Redis
(3)Memcached
(4)ZooKeeper 这个自带了和进程关联的数据管理

由于我希望这个框架，可以让程序自由的选用不同的缓冲存储“设备”，比如在测试的时候，可以不按照任何其他软件，
直接用自己的内存做缓冲，而在运营或者其他情况下，可以使用 Redis 或其他的设备。所以我们可以编写代码来实现
上面的 DataMap 接口，以实现不同的缓冲存储方案。当然必须要把最简单的，使用堆内存的实现完成： RamMap

*/
