#pragma once

// ログレベルを定義
enum severity_level
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string);
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

// ログレベルの表示
std::ostream &operator<<(std::ostream &strm, severity_level level);
void set_log_level(severity_level log_level);
std::string  del_newline_code(const char* log_str);
std::string get_now_str();
void system_log_init();
void system_log(severity_level log_level, const char *fmt, ...);
void access_log_init();
void access_log(severity_level log_level, const char *fmt, ...);