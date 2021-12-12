# BoostHoney
C++、Boostライブラリを用いた非同期I/OのWebハニーポット

# 特徴
* 非同期I/Oを使用した軽量な動作を目的としている。
* アクセスされたすべてのTCP通信を記録
* HTTPリクエストに対しては、事前にXMLに定めたルールでマッチ＆レスポンスを行う。(詳細は./art/mrr_help.txt参照のこと)
* chrootで影響範囲を縮小

# 動作確認環境
* Ubuntu 20.04.3 LTS & BOOST_LIB_VERSION "1_71" & cmake version 3.16.3
* Arch linux & BOOST_LIB_VERSION "1_76" & cmake version 3.22.1
 
# インストール方法
### 動作条件環境構築
* [Ubuntu](./doc/env_setting_ubuntu.md)

### BoostHoney構築方法
```bash
# boost_honeypotのプロジェクトフォルダへ移動
cd <クローンしたboost_honeypotのパス> # ex) /home/user01/boost_honeypot

# log出力先フォルダ作成
mkdir ./log

# chroot下で利用するタイムゾーンの設定
mkdir ./etc
cp -p /etc/localtime ./etc/.

# server.confの編集
vim ./etc/server.conf
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊
[server]
port = <リクエストの受付ポート>
docroot = <クローンしたboost_honeypotのパス>
user = <chrootで使用するユーザ>
group = <chrootで使用するグループ>
syslogfile = <システムログ出力先 (chroot環境でのパス)>
accesslogfile = <アクセスログ出力先 (chroot環境でのパス)>
accesslogsuffix = <アクセスログの接尾辞 (boost::logの仕様に基づく)>
accesslogrotationsize = <アクセスログのローテーションサイズ (Bytes)>
mrrulesfile = <HTTPマッチングに使用するルールファイル (chroot環境でのパス) (詳細は./art/mrr_help.txt参照のこと)>
responsecodeconffile = <HTTPレスポンスコードとメッセージの対応表 (chroot環境でのパス)>
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊

# ハニーポット用ユーザ作成 (chrootで使用するための非特権ユーザ)
sudo groupadd boosthoney
sudo useradd -g boosthoney boosthoney

# ファイル読み書き用にファイル所有者変更
sudo chown -R boosthoney:boosthoney etc
sudo chown -R boosthoney:boosthoney art
sudo chown -R boosthoney:boosthoney log

# ビルド実行
mkdir build
cd build
cmake ..
cmake --build .

# ヘルプ
./boosthoney --help

# 起動
sudo ./boosthoney --conf=<クローンしたboost_honeypotのパス>/etc/server.conf # ex) /home/user01/boost_honeypot/etc/server.conf

```
### systemdでの起動（任意）
```bash
# systemdサービス作成フォルダ確認
ls -l /etc/systemd/system/
# boosthoney用サービス作成
sudo vim /etc/systemd/system/boosthoney.service

<作成例>
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊
[Unit]
Description = boosthoney daemon

[Service]
ExecStart = <boosthoney実行ファイルのフルパス>/ --conf=<クローンしたboost_honeypotのパス>/etc/server.conf
Restart = always
Type = simple
RemainAfterExit=yes

[Install]
WantedBy = multi-user.target
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊

# boosthoneyサービスが作成されたことを確認
systemctl list-unit-files --type=service | grep boosthoney
# boosthoney起動
systemctl start boosthoney
# boosthoneyステータス確認
systemctl status boosthoney
# boosthoneyの自動起動ON
systemctl enable boosthoney
```

# デモ
![DEMO](./doc/demo.gif)

# License
This software is released under the BSD License, see LICENSE.

[LICENSE](./LICENSE)
