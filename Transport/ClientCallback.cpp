
#include <iostream>
using namespace std;
/**
   有了 Server 类型，肯定也需要有 Client 类型。而 Client 类型的设计和 Server 类似，
  但就不是使用 Transport 接口作为传输层，而是 Connector 接口。不过 Protocol 的抽
  象层是完全重用的。Client 并不需要 Processor 这种形式的回调，而是直接传入接受数据
  消息就发起回调的接口对象 ClientCallback。
*/

class ClientCallback {

public:
  
    ClientCallback() {
    }
    virtual ~ClientCallback() {
         // Do nothing
    }

    /**
     *  当连接建立成功时回调此方法。
     * @return 返回 -1 表示不接受这个连接，需要关闭掉此连接。
     */
    virtual int OnConnected() {
        return 0;
    }

    /**
     * 当网络连接被关闭的时候，调用此方法
     */
    virtual void OnDisconnected() {        // Do nothing
    }

    /**
     * 收到响应，或者请求超时，此方法会被调用。
     * @param response 从服务器发来的回应
     * @return 如果返回非0值，服务器会打印一行错误日志。
     */
    virtual int Callback(const Response& response) {
        return 0;
    }

    /**
     * 当请求发生错误，比如超时的时候，返回这个错误
     * @param err_code 错误码
     */
    virtual void OnError(int err_code){
        WARN_LOG("The request is timeout, err_code: %d", err_code);
    }

    /**
     * 收到通知消息时，此方法会被调用
     */
    virtual int Callback(const Notice& notice) {
        return 0;
    }

    /**
     * 返回此对象是否应该被删除。此方法会被在 Callback() 调用前调用。
     * @return 如果返回 true，则会调用 delete 此对象的指针。
     */
    virtual bool ShouldBeRemoved() {
        return false;
    }
};

class Client : public Updateable {
 
public:
    Client();   
    virtual ~Client();

     /**
     * 连接服务器
     * @param connector 传输协议，如 TCP， UDP ...
     * @param protocol 分包协议，如 TLV, Line, TDR ...
     * @param notice_callback 收到通知后触发的回调对象，如果传输协议有“连接概念”（如TCP/TCONND），建立、关闭连接时也会调用。
     * @param config 配置文件对象，将读取以下配置项目：
       MAX_TRANSACTIONS_OF_CLIENT 客户端最大并发连接数;
       BUFFER_LENGTH_OF_CLIENT客户端收包缓存；
       CLIENT_RESPONSE_TIMEOUT 客户端响应等待超时时间。
     * @return 返回 0 表示成功，其他表示失败
     */
    int Init(Connector* connector, Protocol* protocol,
             ClientCallback* notice_callback = NULL, Config* config = NULL);

    /**
     * callback 参数可以为 NULL，表示不需要回应，只是单纯的发包即可。
     */
    virtual int SendRequest(Request* request, ClientCallback* callback = NULL);

    /**
     * 返回值表示有多少数据需要处理，返回-1为出错，需要关闭连接。返回0表示没有数据需要处理。
     */
    virtual int Update();
    virtual void OnExit();
    void Close();
    Connector* connector() ;
    ClientCallback* notice_callback() ;
    Protocol* protocol() ;
};

