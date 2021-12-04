#include "all.hpp"

void http_request::read_header_field(const std::string _request_header_line)
{
    system_log(severity_level(DEBUG), "http_request::read_header_field start!! args _request_header_line=%s", _request_header_line.c_str());
    std::stringstream ss{_request_header_line};
    std::string buf;
    int colon_position = _request_header_line.find(":");
    if (colon_position == (int)std::string::npos)
    {
        system_log(severity_level(ERROR), "parse error on request header line: %s", _request_header_line.c_str());
    }
    
    buf = _request_header_line;
    std::string name = buf.erase(colon_position);
    buf = _request_header_line;
    std::string value = buf.substr(colon_position + 1);
    header_.emplace(name,value);

    system_log(severity_level(DEBUG), "http_request::read_header_field end!!%s", "");
}

void http_request::read_request_line(std::string _request_line)
{
    system_log(severity_level(DEBUG), "http_request::read_request_line start!! args _request_line=%s", _request_line.c_str());
    std::vector<std::string> v;
    std::stringstream ss{_request_line};
    std::string buf;
    while (std::getline(ss, buf, ' '))
    {
        v.push_back(buf);
    }
    if (v.size() < 3){
        system_log(severity_level(ERROR), "parse error on request line: %s", _request_line.c_str());
    }
    std::string method = v[0];
    transform(method.begin(),method.end(), method.begin(), toupper);
    method_ = method;
    path_ = v[1];
    protocol_version_ = v[2];

    system_log(severity_level(DEBUG), "http_request::read_request_line end!!%s", "");
}

http_request::http_request(){}

http_request::http_request(const char* request_data)
{
    system_log(severity_level(DEBUG), "http_request::http_request start!! args request_data=%s", request_data);
    std::stringstream ss{(std::string)request_data};
    std::string buf;
    read_flg f = read_flg(request_line);
    while (std::getline(ss, buf))
    {
        if (buf[buf.size()-1] == '\r') buf.erase(buf.size()-1);
        switch(f)
        {
            case 0: // リクエストラインの読込み
                if(buf != "")
                {
                    read_request_line(buf);
                    f = read_flg(header_field);
                }
                break;
            case 1: // ヘッダーフィールドの読込み
                if(buf == "") //空行以降はボディの読込みへ
                {
                    f = read_flg(body);
                }
                else
                {
                    read_header_field(buf);
                }
                break;
            default: // ボディの読込み
                body_ += buf;
                break;
        }
    }
    system_log(severity_level(DEBUG), "http_request::http_request end!!%s", "");
}
// リクエストで指示されたHTTPバージョン。
std::string http_request::get_protocol_version()
{
    return this->protocol_version_;
}
// リクエストメソッド（GET、HEAD、POSTなど）を格納。常に大文字で格納
std::string http_request::get_method()
{
    return this->method_;
}
// リクエストのパス
std::string http_request::get_path()
{
    return this->path_;
};
// HTTPヘッダリンクリスト
std::map<std::string, std::string> http_request::get_header()
{
    return this->header_;
}
// エンティティボディ
std::string http_request::get_body()
{
    return this->body_;
}
// エンティティボディの長さ
long http_request::get_length()
{
    return this->length_;
}