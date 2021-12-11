#pragma once

class read_response_conf
{
private:
    static read_response_conf* _read_response_conf;
    std::map<std::string, std::string> _response_code_list;
    read_response_conf();
public:
    static read_response_conf* getInstance();
    const std::map<std::string, std::string> get_response_code_list();
    ~read_response_conf();
};