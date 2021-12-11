#include "all.hpp"

tcp_connection::tcp_connection(boost::asio::io_context& io_context)
  : socket_(io_context)
{
    system_log(severity_level(DEBUG), "tcp_connection::tcp_connection start!! %s", "");
    system_log(severity_level(DEBUG), "tcp_connection::tcp_connection end!! %s", "");
}   
void tcp_connection::handle_write(const boost::system::error_code& error)
{
    system_log(severity_level(DEBUG), "tcp_connection::handle_write start!! %s", "");
    if (error && error != boost::asio::error::eof) 
    {
        system_log(severity_level(ERROR), "receive failed: %s", error.message().c_str());
    }
    system_log(severity_level(DEBUG), "tcp_connection::handle_write end!! %s", "");
}

boost::shared_ptr<tcp_connection> tcp_connection::create(boost::asio::io_context& io_context)
{
    system_log(severity_level(DEBUG), "tcp_connection::create end!! %s", "");
    system_log(severity_level(DEBUG), "tcp_connection::create start!! %s", "");
    return boost::shared_ptr<tcp_connection>(new tcp_connection(io_context));
}   
// ソケットの参照を返す
boost::asio::ip::tcp::socket& tcp_connection::socket()
{
    system_log(severity_level(DEBUG), "tcp_connection::socket start!! %s", "");
    system_log(severity_level(DEBUG), "tcp_connection::socket end!! %s", "");
    return socket_;
}
std::string tcp_connection::do_file_response(http_request *req)
{
    system_log(severity_level(DEBUG), "tcp_connection::do_file_response start!! %s", "");
    http_response *resp = new http_response(req);
    system_log(severity_level(DEBUG), "tcp_connection::do_file_response end!! %s", "");
    return resp->get_response_message(req);
}

// リクエスト読込み処理
// error : エラー情報
// bytes_transferred : 受信したバイト数
void tcp_connection::on_receive(const boost::system::error_code& error)
{
    system_log(severity_level(DEBUG), "tcp_connection::on_receive start!! %s", "");
    if (error && error != boost::asio::error::eof) {
        system_log(severity_level(ERROR), "receive failed: %s", error.message().c_str());
    }
    else {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buff_.data());
        access_log(severity_level(INFO), "[%s] %s", socket_.remote_endpoint().address().to_string().c_str(), data);
        http_request *req;
        if (std::string(data).find("HTTP") == std::string::npos)
        {
            req = new http_request();
        }
        else
        {
            req = new http_request(data);
        }
        if(data)
        {
            message_ = do_file_response(req);
            // 第一引数：socketオフジェクトへの参照
            // 第二引数：送信バッファ
            // 第三引数：送信成功時oe失敗時に呼ばれる関数
            boost::asio::async_write(socket_, boost::asio::buffer(message_),boost::asio::transfer_all(),
                // shared_from_this()：自身のオブジェクトを取得する(start()を実行しているオブジェクト)
                boost::bind(&tcp_connection::handle_write, shared_from_this(), boost::asio::placeholders::error));
        }
        receive_buff_.consume(receive_buff_.size());
    }
    system_log(severity_level(DEBUG), "tcp_connection::on_receive end!! %s", "");
}
// 通信処理開始
void tcp_connection::start()
{
    system_log(severity_level(DEBUG), "tcp_connection::start start!! %s", "");
    // リクエスト読込み処理
    boost::asio::async_read(socket_, receive_buff_.prepare(1024), boost::asio::transfer_at_least(4),
        boost::bind(&tcp_connection::on_receive, shared_from_this(), boost::asio::placeholders::error));
    system_log(severity_level(DEBUG), "tcp_connection::start end!! %s", "");
}