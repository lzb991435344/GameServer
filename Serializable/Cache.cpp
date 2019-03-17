#include <iostream>

/**
 * 定义几种网络缓冲模型
 */
enum CacheType
{
	TypeLocal,//本地
	TypeName,//按cache名字分布
	TypeKey,//先按cache的名字分布，再按key分布
	TypeCopy  //复制型分布
};


/**
 * @brief 定义了分布式缓存对象的基本结构
 */
class Cache: public DataMap
{
public:

   /**
     * @brief 连接集群。
     * @return 返回是否连接成功。
     */
	static bool EnsureCluster(Config* config = NULL);


/**
     * 获得一个 Cache 对象。
     * @attention 如果第一次调用此函数时，输入的 type, local_data_map 会成为这个 Cache 的
     *   固定属性，以后只要是 name 对的上，都会是同一个 Cache 对象。
     * @param name 为名字
     * @param type 此缓存希望是哪种类型
     * @param local_data_map 为本地存放的数据容器。
     * @return 如果是 NULL 表示已经达到 Cache 数量的上限。
     */
	static Cache* GetCache(const std::string& name, CacheType type, DataMap* local_data_map);

  
   /**
     * 获得一个 Cache 对象。
     * @param  name 为名字
     * @param local_data_map 为本地存放的数据容器。
     * @note 如果第一次调用此函数时，输入的 type, class M 会成为这个 Cache 的固定属性，以后只要是 name 对的上，都会是同一个 Cache 对象。
     * @return 如果是 NULL 表示已经达到 Cache 数量的上限。
     */
  template <class M>
	static Cache* GetCache(const std::string& name, CacheType type, int* data_map_arg1 = NULL)
	{
		DataMap* local_data_map = NULL;
		std::map<std::string, DataMap*>::iterator it = cache_store_.find(name);
		if(it != cache_store_.end())
		{
			local_data_map = it->second;
		}else
		{
			if(data_map_arg1 != NULL)
			{
				local_data_map = new M(*data_map_arg1);
			}else
			{
				local_data_map = new M();
			}
			cache_store_[name] = local_data_map;
		}
		return GetCache(name, type, local_data_map);
	}


   /**
     * 删除掉本进程内存放的 Cache
     */
	static void RemoveCache(const std::string& name);

	explict Cache(const std::string& name);
	virtual ~Cache();
	virtual std::string GetName() const;
	
protected:
	static std::map<std::string, Cache*> cache_map_;
	std::string name_;
private:
	static int MAX_CACHE_NUM;
	static std::map<std::string,DataMap*> cache_store_;
};
