#pragma once
class http_request {
private:
    enum read_flg
    {
        request_line,
        header_field,
        body
    };
    // リクエストで指示されたHTTPバージョン。
    std::string protocol_version_;
    // リクエストメソッド（GET、HEAD、POSTなど）を格納。常に大文字で格納
    std::string method_;
    // リクエストのパス
    std::string path_;
    // HTTPヘッダリンクリスト
    std::map<std::string, std::string> header_;
    // エンティティボディ
    std::string body_;
    // エンティティボディの長さ
    long length_;

    void read_header_field(const std::string _request_header_line);
    void read_request_line(std::string _request_line);
public:
    http_request();
    http_request(const char* request_data);
    // リクエストで指示されたHTTPバージョン。
    std::string get_protocol_version();
    // リクエストメソッド（GET、HEAD、POSTなど）を格納。常に大文字で格納
    std::string get_method();
    // リクエストのパス
    std::string get_path();
    // HTTPヘッダリンクリスト
    std::map<std::string, std::string> get_header();
    // エンティティボディ
    std::string get_body();
    // エンティティボディの長さ
    long get_length();
    ~http_request();
};