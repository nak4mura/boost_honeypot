#pragma once
class http_response {
private:
    // レスポンスに設定するHTTPバージョン。
    std::string protocol_version_;
    // レスポンスステータス
    std::string status_;
    // HTTPヘッダリスト
    std::map<std::string, std::string> header_;
    // レスポンスボディ
    std::string body_;

    std::string add_response_message(std::string);
    bool is_contain(std::string req_val, std::string xml_val);
    void pattern_matching(http_request *_req);    
    // 現在時刻の取得
    std::string make_daytime_string();
    std::string guess_content_type();
    void set_header_fields(http_request *req);
public:
    http_response(http_request *req);    
    std::string get_response_message(http_request *req);
    ~http_response();
};