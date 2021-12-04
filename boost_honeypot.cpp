#include "all.hpp"

static void signal_exit(int sig) {
  system_log(severity_level(INFO), "exit by signal %d", sig);
  exit(0);
}

static void trap_signal(int sig, sighandler_t handler) {
  struct sigaction act;
  act.sa_handler = handler; // sa_handlerとsa_sigactionはどちらかしか使えない
  sigemptyset(&act.sa_mask);
  if (sigaction(sig, &act, NULL) < 0) {
    fprintf(stderr, "sigaction() failed: %s", strerror(errno));
    exit(1);
  }
}

// シグナル捕捉設定
static void install_signal_handlers(void) {
  trap_signal(SIGPIPE, signal_exit);
  trap_signal(SIGINT, signal_exit);
  trap_signal(SIGTERM, signal_exit);
}

void setup_environment(std::string root, std::string user, std::string group) {
  struct passwd *pw;
  struct group *gr;

  gr = getgrnam(group.c_str());
  if (!gr) {
    fprintf(stderr, "no such group: %s\n", group.c_str());
    exit(1);
  }
  if (setgid(gr->gr_gid) < 0) {
    perror("setgid(2)");
    exit(1);
  }
  if (initgroups(user.c_str(), gr->gr_gid) < 0) {
    perror("initgroups(2)");
    exit(1);
  }
  pw = getpwnam(user.c_str());
  if (!pw) {
    fprintf(stderr, "no such user: %s\n", user.c_str());
    exit(1);
  }
  chroot(root.c_str());
  if (setuid(pw->pw_uid) < 0) {
    perror("setuid(2)");
    exit(1);
  }
}

void become_daemon(void) {
  int n;

  if (chdir("/") < 0) {
    fprintf(stderr,  "chdir(2) failed: %s", strerror(errno));
    exit(1);
  }

  freopen("/dev/null", "r", stdin);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);
  n = fork();
  if (n < 0) {
    fprintf(stderr, "fork(2) failed: %s", strerror(errno));
    exit(1);
  }
  if (n != 0)
    _exit(0);
  if (setsid() < 0) {
    fprintf(stderr, "setsid(2) failed: %s", strerror(errno));
    exit(1);
  }
  system_log(severity_level(DEBUG), "become_daemon end!! %s", "");
}

int main(int argc, char *argv[]) {
  // オプションの設定
  boost::program_options::options_description description("options");
  description.add_options()
    ("help", "ヘルプを表示")
    ("daemon,m", "自力でデーモン化")
    ("debug,d", "デバッグモードで起動")
  ;

  // オプションの取得
  boost::program_options::variables_map vm;
  try
  {
    boost::program_options::store(parse_command_line(argc, argv, description), vm);
  }
  catch (const boost::program_options::error_with_option_name& e)
  {
    std::cerr << "get options failed. " << e.what() << std::endl;
    exit(1);
  }
  
  boost::program_options::notify(vm);

  // オプションhelpが存在すればdescriptionを出力して終了
  if (vm.count("help")) {
    std::cout << description << std::endl;
    exit(0);
  }

  // server.conf 読込み
  read_server_conf* server_conf = read_server_conf::getInstance(boost::filesystem::system_complete(argv[0]).parent_path());
  int port;
  std::string docroot;
  std::string user;
  std::string group;
  try {
    port = atoi(server_conf->get_server_list().at("port").c_str());
    docroot = server_conf->get_server_list().at("docroot");
    user = server_conf->get_server_list().at("user");
    group = server_conf->get_server_list().at("group");
  }
  catch(std::out_of_range&)
  {
    std::cerr << "failed to read server.conf" << std::endl;
    exit(1);
  }

  // docrootに移動
  chdir(docroot.c_str());

  // オプションchroot・user・groupが存在すればchrootの環境設定
  setup_environment(docroot, user, group);
  docroot = "";
  
  install_signal_handlers();

  // オプションdaemonが存在すればプロセスを自力デーモン化
  if (vm.count("daemon")) {
    become_daemon();
  }  

  // ログ初期化
  system_log_init();
  access_log_init();

  if (vm.count("debug"))
  {
    set_log_level(DEBUG);
  }
  else
  {
    set_log_level(INFO);
  }

  system_log(severity_level(DEBUG), "main start!!%s", "");   
  
  // サーバ起動
  try
  {
    // I/O実行コンテクストを作成
    boost::asio::io_context io_context;
    // TCPサーバの初期化
    tcp_server server(io_context, port);
    // 非同期のキューがすべて完成されるまで実行を続ける
    io_context.run();
  }
  catch (std::exception &e)
  {
    system_log(severity_level(ERROR), "server launch failes. %s", e.what());
  }  
  system_log(severity_level(INFO), "main end!! %s", "");
  return 1;
}