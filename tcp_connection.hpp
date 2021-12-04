#pragma once
// boost::enable_shared_from_thisクラスを継承
// enable_shared_from_this：クラスのメンバ関数内でshared_from_this関数により自身のshared_ptrを取得することができる
// コンストラクタでsocketにio_contextをセット
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
private:
    tcp_connection(boost::asio::io_context& io_context);
    void handle_write(const boost::system::error_code& error);
    boost::asio::ip::tcp::socket socket_;
    std::string message_;
    boost::asio::streambuf receive_buff_;

public:
    // 新しい型pointerを宣言←boost::shared_ptd<tcp_connection> (スマートポインタ)
    //typedef boost::shared_ptr<tcp_connection> pointer;
    // io_contextをセットした新しいtcp_connectionのシェアードポインタを返す
    static boost::shared_ptr<tcp_connection> create(boost::asio::io_context& io_context);
    // ソケットの参照を返す
    boost::asio::ip::tcp::socket& socket();
    std::string do_file_response(http_request *req);    
    // リクエスト読込み処理
    // error : エラー情報
    // bytes_transferred : 受信したバイト数
    void on_receive(const boost::system::error_code& error); //, size_t bytes_transferred)
    // 通信処理開始
    void start();
};