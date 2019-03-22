#include <iostream>

/**
 * @brief 由于持久化操作一般都是耗时等待的操作，所以需要回调接口来通知各种操作的结果。
 */

class DataStoreCallback
{
public:
	static int MAX_HNAG_UP_CALLBACK_NUM;//最大回调挂起数
	std::string key;
	Serializable* value;

	DataStoreCallback();
	virtual ~DataStoreCallback();


  /**
     * 当 Init() 初始化结束时会被调用。
     * @param  result 是初始化的结果。0 表示初始化成功。
     * @param  msg 是初始化可能存在的错误信息。可能是空字符串 ""。
     */
  virtual void OnInit(int result, const std::string& msg);


  /**
     * 当调用 Get() 获得结果会被调用
     * @param key
     * @param value
     * @param result 是 0 表示能获得对象，否则 value 中的数据可能是没有被修改的。
     */
   virtual void OnGot(const std::string& key, Serializable* value, int result);


  /**
     * 当调用 Put() 获得结果会被调用。
     * @param key
     * @param result 如果 result 是 0 表示写入成功，其他值表示失败。
     */
   virtual void OnPut(const std::string& key, int result);


   /**
     * 当调用 Remove() 获得结果会被调用
     * @param key
     * @param result 返回 0 表示删除成功，其他值表示失败，如这个 key 代表的对象并不存在。
     */
   virtual void OnRemove(const std::string& key, int result);


   /**
     * 准备在发起用户设置的回调，如果使用者没有单独为一个回调事务设置单独的回调对象。
     * 在 PrepareRegisterCallback() 中会生成一个临时对象，在此处会被串点参数并清理。
     * @param privdata 由底层回调机制所携带的回调标识参数，如果为 NULL 则返回 NULL。
     * @param init_cb 初始化时传入的共用回调对象
     * @return 可以发起回调的对象，如果为 NULL 表示参数 privdata 是 NULL
     */
   static DataStoreCallback* PrepareUseCallback(void* privdata, DataStoreCallback* init_cb);


   /**
     * 检查和登记回调对象，以防内存泄漏。
     * @param callback 可以是NULL，会新建一个仅仅用于存放key数据的临时callback对象。
     *  @return 返回一个callback对象，如果是 NULL 表示有太多的回调过程未被释放。
     */
   static DataStoreCallback* PrepareRegisterCallback(DataStoreCallback* callback);
protected:
	static int kHandupCallbacks;//有多少个回调指针被挂起，达到上限会停止工作
};


/**
 *@brief 用来定义可以持久化对象的数据存储工具接口
 */

class DataStore : public Component
{
public:
	DataStore(DataStoreCallback* callback = NULL):callback_(callback){ }

	virtual ~DataStore(){ }


   /**
     * 初始化数据存取设备的方法，譬如去连接数据库、打开文件之类。
     * @param config 配置对象
     * @param callback 参数 callback 为基本回调对象，初始化的结果会回调其 OnInit() 函数通知用户。
     * @return 基本的配置是否正确，返回 0 表示正常。
     */
	virtual int Init(Config* config, DataStoreCallback* callback);

	virtual int Init(Application* app, Config* cfg)
	{
		app_ = app;
		return Init(cfg, callback_);
	}

	virtual std::string GetName()
	{
		return "den::DataStore";
	}

	virtual int Stop()
	{
		Close();
		return 0;
	}

	/**
     * 驱动存储接口程序运行，触发回调函数。
     * @return 返回 0 表示此次没有进行任何操作，通知上层本次调用后可以 sleep 一下。
     */
	virtual int Update() = 0;


   /**
     * 关闭程序，关闭动作虽然是异步的，但不再返回结果，直接关闭就好。
     */
	virtual int Close() = 0;


   /**
     * 读取一个数据对象，通过 key ，把数据放入到 value，结果会回调通知 callback。
     * 发起调用前，必须把 callback 的 value 字段设置为输出参数。
     * @param key
     * @param callback 如果NULL，则会回调从 Init() 中传入的 callback 对象。
     * @return 0 表示发起请求成功，其他值为失败
     */
	virtual int Get(const std::string& key, DataStoreCallback* callback = NULL) = 0;


   /**
     * 写入一个数据对象，写入 key ，value，写入结果会回调通知 callback。
     * @param key
     * @param value
     * @param callback 如果是 NULL，则会回调从 Init() 中传入的 callback 对象。
     * @return 表示发起请求成功，其他值为失败
     */
    virtual int Put(const std::string& key, const Serializable& value,DataStoreCallback* callback = NULL) = 0;



   /**
     * 删除一个数据对象，通过 key ，结果会回调通知 callback。
     * @param key
     * @param callback 如果是 NULL，则会回调从 Init() 中传入的 callback 对象。
     * @return 表示发起请求成功，其他值为失败
     */
    virtual int Remove(const std::string& key, DataStoreCallback* callback = NULL) = 0;

protected:
	//存放初始化传入的回调指针
	DataStoreCallback* callback_;
};

/**
针对上面的 DataStore 模型，可以实现出多个具体的实现：
(1)文件存储(一定需要)
(2)Redis
(3)其他的各种数据库
	在游戏的持久化数据里面，还有两类功能是比较常用的，一种是排行榜的使用；另外一种是拍卖行。这两个功能
是基本的 Key-Value 无法完成的。使用 SQL 或者 Redis 一类 NOSQL 都有排序功能，所以实现排行榜问题不大。
而拍卖行功能，则需要多个索引，所以只有一个索引的 Key-Value NoSQL 是无法满足的。不过 NOSQL 也可以
“手工”的去建立多个 Key 的记录。

*/