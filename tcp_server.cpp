#include "all.hpp"
// コンストラクタ
tcp_server::tcp_server(boost::asio::io_context& io_context, int port)
    : io_context_(io_context), acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    system_log(severity_level(DEBUG), "tcp_server::tcp_server start!! %s", "");
    start_accept();
    system_log(severity_level(DEBUG), "tcp_server::tcp_server end!! %s", "");
}

// 新規コネクションでの接続待機開始
void tcp_server::start_accept()
{
    system_log(severity_level(DEBUG), "tcp_server::start_accept start!! %s", "");
    boost::shared_ptr<tcp_connection> new_connection = tcp_connection::create(io_context_); 
    // 第一引数：バインディングするsocketオブジェクトへの参照
    // 第二引数：接続成功・失敗時に呼ばれる関数
    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
    system_log(severity_level(DEBUG), "tcp_server::start_accept end!! %s", "");
}   

// 通信処理開始・新規コネクションでの接続待機開始
void tcp_server::handle_accept(boost::shared_ptr<tcp_connection> new_connection, const boost::system::error_code& error)
{
    system_log(severity_level(DEBUG), "tcp_server::handle_accept start!! %s", "");
    if (!error)
    {
        new_connection->start();
    }
    start_accept();
    system_log(severity_level(DEBUG), "tcp_server::handle_accept end!! %s", "");
}