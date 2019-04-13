
#include <iostream>
#include <string.h>

/**
 * @brief 集群中心客户端
 * 每个 DenOS 进程启动时，都会向 ZooKeeper 注册自己的服务。
 * 此类型的对象，就是作为每个进程中，代表集群中心的存在。
 * ZooKeeper 默认根据 2 个 tick （心跳），大概为 3 秒，是否收到，来决定客户端是否死掉。
 * 在 DenOS 中，可以使用配置项目 ZK_RECV_TIMEOUT 参数（单位为毫秒）来决定这个超时时间。
 */
 
 class Center:public ZooKeeperMap{
	 friend void ContractsWatcher();
public:
	int last_error_code_; //用来测试最后一个操作是否成功的变量
	//key服务名字,value节点的名字
	std::map<std::string, std::string> service_process_;
	
	Center(const std::string& urls = "127.0.0.1:2108");
	virtual ~Center();
	
	//驱动的异步流程
	virtual int Update();
	
	 
	/**
     * 往集群中声明注册服务
     * @param name 服务的名字
     * @param contract 服务的通信方式
     * @return 返回 0 表示已经发起注册流程，其他值表示失败
     */
	 int RegisterService(const std::string& name, const Contract& contract);
	 
	  
   /**
     * @brief 注册一个类作为对应协议字符串名字
     * 如果这个类是 Connector 的子类，必须要有一个形如 XXConnector(const string& p1, int p2) 这样的构造器。
     * 或者这个类是 Protocol 的子类，必须要有一个无参数构造器。
     * @param reg_name 协议字符串名字，如 tcp/udp/kcp/tconnd 或者 tlv/line/tdr
	 */
	 template<typename T>
	 void RegProto(const std::string& reg_name){
		 if(reg_name.empty()){
			 return ;
		 }
		 constructors_[reg_name] = new DefaultConstructor<T>();
	 }
	 
	 template<typename T>
	 void RegConn(const std::string& reg_name){
		if(reg_name.empty()){
			 return ;
		}
		 constructors_[reg_name] = new StrInConstructor<T>();
	 }
	  
    /**
     * @brief 查询一个服务去发起请求
     * 注意这是一个异步的接口，有可能会返回 -1 表示服务合同还未拿到。需要重复的去获取。
     * @param name 服务的名字
     * @param callback 当获得对应的服务的客户端的回调
     * @param client_cb 预期每个新的 Client 所注册的默认回调，用来接收连接、中断、收听通知。
     * @param route_param 用来传给路由器的自定义路由相关数据
     * @return  如果返回 0 表示成功，失败则会是其他数值
     */
	 int QueryService(const std::string& name, GetServiceClientCallback* callback,
	                  ClientCallback* client_cb = NULL, Router* router = NULL,
					  void* route_param = NULL);
	
	/**
     * 根据合同缓存获得客户端对象
     * @param cache 合同缓存对象
     * @param client_cb 预期每个新的 Client 所注册的默认回调，用来接收连接、中断、收听通知。
     * @param router 路由器对象
     * @param route_param 路由参数
     * @return 客户端对象指针，无需主动 delete，因为会缓存起来
     */
	 Client* GetClientByContracts(ContractCache* cache, ClientCallback* client_cb,
								Router* router, void* route_param);
	 
    /**
     * 获得存放集群的 ZK 基础路径
     * @return ZNode 基础路径
     */
	 inline const std::string& cluster_prefix() const{
		 return CLUSTER_PREFIX;
	 }
	 
	 
    /**
     * 获得建立进程用的 ZK 标记 (Create flags)
     * @return zk 的 create flags
     */
	 inline int process_flags() const{
		 return process_flags;
	 }
	 // 在 ZK 写入本进程对此服务的合约
	 void AddProcessContract(const std::string& service_name, 
							const std::string& contract_data);
	 //清理相关的对象
	 void CloseClient(Client* client);
	 
	 //设置合同缓存
	 void SetContractsCache(const std::string& service_name,ContractCache& cache);
	 
	 // 建立存储节点父目录时，增加一个监听器，监听这些节点增加和删除变化
	 virtual void CreatePrefixNode();
	 
	 // 初始化 zookeeper 客户端连接，会修改 ZKMAP_KEY_PREFIX 为集群专用路径
	 virtual int Init(Config* config = NULL);
	 
	 
	/**
     * 清理掉生成的客户端对象
     * @param client 客户端对象
     * @param content 相关的服务名字
     */
	 void ClearClientMember(Client* client, const std::string& content);
	 
private:	 
 };