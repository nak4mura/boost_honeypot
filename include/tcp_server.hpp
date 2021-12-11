#pragma once
// tcp_serverオブジェクト作成
// コンストラクタでコネクションの作成と新規アクセプターの作成
class tcp_server
{
public:
    // コンストラクタ
    tcp_server(boost::asio::io_context& io_context, int port);
private:
    // 新規コネクションでの接続待機開始
    void start_accept();  
    // 通信処理開始・新規コネクションでの接続待機開始
    void handle_accept(boost::shared_ptr<tcp_connection> new_connection, const boost::system::error_code& error);
    void on_receive(const boost::system::error_code& error, size_t bytes_transferred);    // I/O実行コンテクスト
    boost::asio::io_context& io_context_;
    // tcp受信部オブジェクト
    boost::asio::ip::tcp::acceptor acceptor_;
};