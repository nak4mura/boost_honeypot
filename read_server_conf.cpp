#include "all.hpp"

read_server_conf* read_server_conf::_read_server_conf = nullptr;
read_server_conf::read_server_conf(boost::filesystem::path exec_path)
{
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini( exec_path.string() + FILE_NAME, pt);
    for (auto& section : pt)
    {
        for (auto& key : section.second)
        {
            _server_list.emplace(key.first,key.second.get_value<std::string>());
        }
    }
};

read_server_conf* read_server_conf::getInstance()
{
    return _read_server_conf;
};

read_server_conf* read_server_conf::getInstance(boost::filesystem::path exec_path)
{
    if(!_read_server_conf)
    {
        _read_server_conf = new read_server_conf(exec_path);
    }
    return _read_server_conf;
};

const std::map<std::string, std::string> read_server_conf::get_server_list()
{
    return this->_server_list;
};

read_server_conf::~read_server_conf(){};
