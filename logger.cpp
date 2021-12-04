#include "all.hpp"

// ログレベルの表示
std::ostream &operator<<(std::ostream &strm, severity_level level)
{
    static const char *strings[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
    {
        strm << strings[level];
    }
    else
    {
        strm << static_cast<int>(level);
    }
    return strm;
}

// ログに含まれる改行を削除
std::string del_newline_code(const char* log_str)
{
    std::string buf_str = log_str;
    std::string ret_str = "";
    for (const auto c : buf_str) {
        if (c == '\r')
            continue;
        else if (c == '\n')
            ret_str.push_back('\t');
        else
            ret_str.push_back(c);
    }
    return ret_str;
}

void set_log_level(severity_level log_level)
{
    boost::log::core::get()->set_filter(severity >= log_level);
}

std::string get_now_str()
{
  char date[64];
  time_t t = time(NULL);
  strftime(date, sizeof(date), "%Y%m%d%H%M%S", localtime(&t));
  return (std::string) date;
}

void system_log_init()
{
    // server.conf 読込み
    read_server_conf* server_conf = read_server_conf::getInstance();
    std::string syslogfile;
    try {
      syslogfile = server_conf->get_server_list().at("syslogfile");
    }
    catch(std::out_of_range&)
    {
      std::cerr << "failed to read server.conf" << std::endl;
      exit(1);
    }

    typedef boost::log::sinks::text_ostream_backend text_backend;
    typedef boost::log::sinks::synchronous_sink<text_backend> text_sink;
    boost::shared_ptr<text_backend> backend = boost::make_shared<text_backend>();
    backend->auto_flush(true);
    // sinkの初期化
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>(backend);
    // systemログのストリーム作成
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(syslogfile + "_" + get_now_str() + ".log"));
    // systemログのフォーマット
    boost::log::formatter sys_fmt = boost::log::expressions::stream
                                    << severity << "\t"
                                    << boost::log::expressions::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S.%f") << "\t"
                                    << boost::log::expressions::smessage;
    // systemログのsink設定
    sink->set_formatter(sys_fmt);
    sink->set_filter(!(boost::log::expressions::has_attr(tag_attr) && tag_attr == "ACCESS_LOG"));
    boost::log::core::get()->add_sink(sink);

    boost::log::add_common_attributes();
}

void access_log_init()
{
    // server.conf 読込み
    read_server_conf* server_conf = read_server_conf::getInstance();
    std::string accesslogfile;
    std::string accesslogsuffix;
    std::string accesslogrotationsize;
    try {
      accesslogfile = server_conf->get_server_list().at("accesslogfile");
      accesslogsuffix = server_conf->get_server_list().at("accesslogsuffix");
      accesslogrotationsize = server_conf->get_server_list().at("accesslogrotationsize");
    }
    catch(std::out_of_range&)
    {
      std::cerr << "failed to read server.conf" << std::endl;
      exit(1);
    }

    typedef boost::log::sinks::text_file_backend text_file_backend;
    boost::shared_ptr<text_file_backend> text_backend =
    boost::make_shared<text_file_backend>(
      boost::log::keywords::file_name = accesslogfile + accesslogsuffix + ".log",
      boost::log::keywords::rotation_size = stol(accesslogrotationsize),
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0)
    );

    text_backend->auto_flush(true);
  
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_text_t;
    boost::shared_ptr<sink_text_t> sink_text(new sink_text_t(text_backend));
  
    boost::log::formatter access_fmt = boost::log::expressions::stream 
                                        << boost::log::expressions::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S.%f") << "\t"
                                        << boost::log::expressions::smessage;
    // accessログのsink設定
    sink_text->set_formatter(access_fmt);
    sink_text->set_filter(boost::log::expressions::has_attr(tag_attr) && tag_attr == "ACCESS_LOG");
    boost::log::core::get()->add_sink(sink_text);
    boost::log::add_common_attributes();
}

void system_log(severity_level log_level, const char *fmt, ...)
{
    char log_str[8192];
    boost::log::sources::severity_logger<severity_level> slg;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log_str, 8192, fmt, ap);
    slg.add_attribute("Tag", boost::log::attributes::constant<std::string>("SYSTEM_LOG"));
    BOOST_LOG_SEV(slg, log_level) << del_newline_code(log_str).c_str();
    va_end(ap);
}

void access_log(severity_level log_level, const char *fmt, ...)
{
    char log_str[8192];
    boost::log::sources::severity_logger<severity_level> slg;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log_str, 8192, fmt, ap);
    slg.add_attribute("Tag", boost::log::attributes::constant<std::string>("ACCESS_LOG"));
    BOOST_LOG_SEV(slg, log_level) << del_newline_code(log_str).c_str();
    va_end(ap);
}
