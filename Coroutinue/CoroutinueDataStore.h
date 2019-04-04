
/**
 * @brief DataStore 的具备协程能力的装饰器类型。
 * @attention 除了定义变量语句和 Update() 以外，其他的操作都需要在协程中调用。
 */
class CoroutinueDataStore:public Updateable
{
public:
	CoroutinueDataStore(DataStore* data_store, CoroutinueDataStore* schdule);
	virtual ~CoroutinueDataStore();
	
	int Init(Config* config, std::string* err_msg);
	
	 
	/**
     * 读取一个数据对象，通过 key ，把数据放入到输出参数 value。
     * 此函数会在调用过程中使用协程的 yield 出去。
     */
	 int Get(const std::string& key, Serializable* value);
	 
	 
	/**
     * 写入一个数据对象，写入 key ，value
     * 写入结果从返回值获得，返回 0 表示成功，其他值表示失败。
     * 此函数会在调用过程中使用协程的 yield 出去。
     */
	 int Put(const std::string& key, const Serializable& value);
	 
	  
    /**
     * 删除一个数据对象，通过 key
     * 写入结果从返回值获得，返回 0 表示成功，其他值表示失败。
     * 此函数会在调用过程中使用协程的 yield 出去
     */
	 int Remove(const string& key);
	 
	 int Update();
	 
protected:
private:
	DataStore* data_store_;
	CoroutinueSchdule* schdule_;
	
};