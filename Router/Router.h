/**
 * @brief 路由器基类
 */
class Router
{
public:
    Router();
    
	virtual  ~Router();

	/**
     * @brief 决定服务路由的接口
     * 此接口默认实现是取 cache 中的第一个非空结果
     * @param cache 需要选择的所有合同的缓存集合
     * @param content 输出参数，具体选择的合同的内容
     * @param route_param 用来提供给路由算法运行的额外参数
     */
	virtualvoidRouteToSevice(const ContractCache& cache, std::string* content,
                             void* route_param = NULL);
	/**
	  路由器的写法非常简单，也附带了一个 route_param 用来帮助传递一些路由选择所需
	的数据。当然你也可以构造多个不同的 Router 子类对象，用对象成员属性来携带更复
	杂的路由参数。	
	*/						 
};


/**
     当我们选择出了合同，就可以利用 Center 的功能去发起服务请求了。下面是单元测试
    的部分代码，展示了如何在服务器之间调用服务：
*/
void TestCallService(){
	
// 获取客户端
	DEBUG_LOG("========== Getting Client ==========");
    
	Center* center = obj_pro_reg.center();
    
	TestGetClientCallback  callback;
    
	TestClientCallback cli_cb;
    center->QueryService(handler.service_name_, &callback, &cli_cb);
    
	for(int i = 0; i < 100; i++) {
        usleep(100);
        svr->Update();
    }
    EXPECT_EQ(0, callback.err_code_);
    EXPECT_TRUE(cli_cb.is_connected_);
    ASSERT_TRUE(callback.client_ != NULL);

    
	// 访问服务器
    DEBUG_LOG("========== Requesting Service ==========");
    
	Client* client = callback.client_;
    
	Request req;
	string data("I love JMX!");
    req.service = handler.service_name_;
    req.SetData(data.c_str(), data.length());
    client->SendRequest(&req, &cli_cb);
    
	for(inti = 0; i < 100; i++) {
        usleep(100);
        svr->Update();
    }
    EXPECT_EQ(data, cli_cb.resp_data_);	
}
