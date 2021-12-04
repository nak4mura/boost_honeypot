#pragma once

class read_server_conf
{
private:
    const std::string FILE_NAME = "/server.conf";
    static read_server_conf* _read_server_conf;
    std::map<std::string, std::string> _server_list;
    read_server_conf(boost::filesystem::path exec_path);
public:
    static read_server_conf* getInstance();
    static read_server_conf* getInstance(boost::filesystem::path exec_path);
    const std::map<std::string, std::string> get_server_list();
    ~read_server_conf();
};