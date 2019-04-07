
#include <iostream>


/**
 * 代表一个应用程序组件，所有的应用程序组件应该继承此对象
 */
 
 class Component: public Updateable
 {
public:
	Component();
	virtual ~Component();
	
	
	/**
     * 返回此组件的名字
     * @return 名字
     */
	 virtual std::string GetName() = 0;
	 
	 
	/**
     * 初始化过程会调用此方法
     * @param app 进程对象
     * @param cfg 配置
     * @return 返回 0 表示成功，其他表示失败
     */
	 virtual int Init(Application* app, Config* config);
	 
	 /**
     * 更新过程会调用此方法
     * @return 返回更新处理的负载，0 表示没有负载，负数表示出错或需要停止对此组件进行更新
     */
	 virtual int Update();
	 
	  
	/**
     * 应用停止时会调用此方法
     * @return 返回 0 表示可以退出，返回 >0 表示需要等待返回值的秒数，返回 < 0 表示出错退出
     */
	 virtual int Stop(){
		 return 0;
	 }
	 
	/**
     * 设置组件被加入的应用程序，用于让组件继承者能简单的获取 Application 对象
     * @note 如果一个组件被加入多个不同的 Application，必须使用 @see Init() 方法来具体保存 Application 对象，
     * 因为此处修改的成员对象 app_ 将是最后一次被添加进的 Application 对象。
     * @param app 要设置的 Application 对象。
     */
	 void set_app(Application* app){
		 app_ = app;
	 }
protected:
	Application* app_;
private:
	
 };