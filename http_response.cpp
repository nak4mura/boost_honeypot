#include "all.hpp"

std::string http_response::add_response_message(std::string key)
{
    system_log(severity_level(DEBUG), "http_response::add_response_message start!! args: key=%s", key.c_str());
    read_response_conf* iniFile = read_response_conf::getInstance();
    std::string message;
    try {
        message = iniFile->get_response_code_list().at(key);
    }
    catch(std::out_of_range&)
    {
        message = "200 OK";
    }
    system_log(severity_level(DEBUG), "http_response::add_response_message end!! return: %s", message.c_str());
    return message;
};

bool http_response::is_contain(std::string req_val, std::string xml_val)
{
    system_log(severity_level(DEBUG), "http_response::is_contain start!! args: req_val=%s, xml_val=%s", req_val.c_str(), xml_val.c_str());
    bool ret_bool = false;
    if(req_val.find(xml_val) != std::string::npos) 
    {
        ret_bool = true;
    }
    system_log(severity_level(DEBUG), "http_response::is_contain start!! return: %s", (ret_bool ? "true" : "false"));
    return ret_bool;
}

void http_response::pattern_matching(http_request *_req)
{
    system_log(severity_level(DEBUG), "http_response::pattern_matching start!!%s", "");
    // server.conf 読込み
    read_server_conf* server_conf = read_server_conf::getInstance();
    std::string mrrulesfile;
    try {
      mrrulesfile = server_conf->get_server_list().at("mrrulesfile");
    }
    catch(std::out_of_range&)
    {
      system_log(severity_level(ERROR), "failed to read server.conf <%s>", "mrrulesfile");
      exit(1);
    }
    boost::property_tree::ptree pt;
    read_xml(mrrulesfile, pt);
    for (auto &mrr : pt.get_child("mrrs"))
    {
        if (mrr.first == "<xmlattr>") continue;
        // トリガーの判定
        if (boost::optional<std::string> xml_val = mrr.second.get_optional<std::string>("meta.enable"))
        {
            std::string enable = xml_val.get();
            std::transform(enable.begin(), enable.end(), enable.begin(), ::tolower);
            bool is_true = enable == "true" ? true : false;
            if(!is_true) continue;
        }
        if (boost::optional<std::string> xml_val = mrr.second.get_optional<std::string>("trigger.uri"))
        {
            std::string pattern = xml_val.get();
            std::regex re(xml_val.get());
            if(!std::regex_search(_req->get_path(),re)) continue;
        }
        if (boost::optional<std::string> xml_val = mrr.second.get_optional<std::string>("trigger.method"))
        {
            if(_req->get_method().find(xml_val.get()) == std::string::npos) continue;
        }
        if (boost::optional<std::string> xml_val = mrr.second.get_optional<std::string>("trigger.header"))
        {
            bool match_flg = false;
            for (const auto& [name, value] : _req->get_header())
            {
                std::string header_line = name + " : " + value;
                if(header_line.find(xml_val.get()) != std::string::npos) match_flg = true;
            }
            if(!match_flg) continue;
        }
        if (boost::optional<std::string> xml_val = mrr.second.get_optional<std::string>("trigger.body"))
        {
            if(_req->get_path().find(xml_val.get()) == std::string::npos) continue;
        }
        // レスポンスの設定
        this->header_.clear();
        this->body_ = "";
        for (auto &res : mrr.second.get_child("response"))
        {
            std::string first_data = res.first.data();
            if (std::equal(begin(first_data), end(first_data), "status"))
            {
                if (boost::optional<std::string> status_val = res.second.get_optional<std::string>(""))
                {
                    this->status_ = add_response_message(status_val.get());
                }
                else
                {
                    this->status_ = "200 OK";
                }
            }
            else if (std::equal(begin(first_data), end(first_data), "header"))
            {
                std::string name;
                std::string value;
                if (boost::optional<std::string> header_name_val = res.second.get_optional<std::string>("name"))
                {
                    name = header_name_val.get();
                }
                if (boost::optional<std::string> header_value_val = res.second.get_optional<std::string>("value"))
                {
                    value = header_value_val.get();
                }
                this->header_.emplace(name,value);
            }
            else if (std::equal(begin(first_data), end(first_data), "body"))
            {
                if (boost::optional<std::string> body_val = res.second.get_optional<std::string>("<xmlattr>.filename"))
                {
                    std::string body_contents;
                    {
                        std::ifstream ifs("./art/" + body_val.get());
                        if (!ifs) {
                            body_contents = "";
                        }
                        std::string buf;
                        while (!ifs.eof())
                        {
                            std::getline(ifs, buf);
                            body_contents += buf + "\n";
                        }
                    }
                    this->body_ = body_contents;
                }
                else if (boost::optional<std::string> body_val = res.second.get_optional<std::string>(""))
                {
                    this->body_ = body_val.get();
                }
            }
        }
    }
    system_log(severity_level(DEBUG), "http_response::pattern_matching end!!%s", "");
}

// 現在時刻の取得
std::string http_response::make_daytime_string()
{
    system_log(severity_level(DEBUG), "http_response::make_daytime_string start!!%s", "");
    std::time_t now = time(0);
    std::string buf = std::ctime(&now);
    if (buf[buf.size()-1] == '\n') buf.erase(buf.size()-1);
    system_log(severity_level(DEBUG), "http_response::make_daytime_string end!! return: %s", buf.c_str());
    return buf;
}

std::string http_response::guess_content_type()
{
    system_log(severity_level(DEBUG), "http_response::guess_content_type start!!%s", "");
    system_log(severity_level(DEBUG), "http_response::guess_content_type end!!%s", "");
    // 固定値として"text/plain"を返却
    return "text/plain";
}

void http_response::set_header_fields(http_request *req)
{
    system_log(severity_level(DEBUG), "http_response::set_header_fields start!!%s", "");
    std::string response_lines;
    this->header_.emplace(std::string("Date"), make_daytime_string());
    this->header_.emplace(std::string("Connection"), std::string("close")); // keep-alive or close
    this->header_.emplace(std::string("Content-Length"), std::to_string(this->body_.length()));
    this->header_.emplace(std::string("Content-Type)"), guess_content_type());
    system_log(severity_level(DEBUG), "http_response::set_header_fields end!!%s", "");
}

http_response::http_response(http_request *req)
{
    system_log(severity_level(DEBUG), "http_response::http_response start!!%s", "");
    this->protocol_version_ = req->get_protocol_version();
    pattern_matching(req);
    set_header_fields(req);
    system_log(severity_level(DEBUG), "http_response::http_response end!!%s", "");
};

std::string http_response::get_response_message(http_request *req)
{
    system_log(severity_level(DEBUG), "http_response::get_response_message start!!%s", "");
    std::string response_message = "";
    response_message += (boost::format("%1% %2%\r\n") % this->protocol_version_ % this->status_).str();
    for (const auto& [name, value] : this->header_)
    {
        response_message += (boost::format("%1%: %2%\r\n") % name % value).str();
    }
    response_message += "\r\n";
    if (req->get_method() != "HEAD") {
        response_message += this->body_;
    }
    system_log(severity_level(DEBUG), "http_response::get_response_message end!! return: %s", response_message.c_str());
    return response_message;
}