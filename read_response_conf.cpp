#include "all.hpp"

read_response_conf* read_response_conf::_read_response_conf = nullptr;
read_response_conf::read_response_conf()
{
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(FILE_NAME, pt);
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
