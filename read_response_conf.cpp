#include "all.hpp"

read_response_conf* read_response_conf::_read_response_conf = nullptr;
read_response_conf::read_response_conf()
{
    boost::property_tree::ptree pt;
    read_server_conf* server_conf = read_server_conf::getInstance();
    std::string responsecodeconffile;
    try {
        responsecodeconffile = server_conf->get_server_list().at("responsecodeconffile");
    }
    catch(std::out_of_range&)
    {
        system_log(severity_level(ERROR), "failed to read server.conf <%s>", "responsecodeconffile");
        exit(1);
    }
    boost::property_tree::read_ini(responsecodeconffile, pt);
    for (auto& section : pt)
    {
        for (auto& key : section.second)
        {
            _response_code_list.emplace(key.first,key.second.get_value<std::string>());
        }
    }
};

read_response_conf* read_response_conf::getInstance()
{
    if(!_read_response_conf)
    {
        _read_response_conf = new read_response_conf();
    }
    return _read_response_conf;
};

const std::map<std::string, std::string> read_response_conf::get_response_code_list()
{
    return this->_response_code_list;
};

read_response_conf::~read_response_conf(){};
