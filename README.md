# BoostHoney
C++、Boostライブラリを用いた非同期I/OのWebハニーポット

# 特徴
* 非同期I/Oを使用した軽量な動作を目的としている。
* アクセスされたすべてのTCP通信を記録
* HTTPリクエストに対しては、事前にXMLに定めたルールでマッチ＆レスポンスを行う。(詳細は./art/mrr_help.txt参照のこと)
* chrootで影響範囲を縮小

# 必要外部ライブラリ
* boost 1.76.0
 
# インストール方法
### 構築方法
```bash
# boost_honeypotのプロジェクトフォルダへ移動
cd <クローンしたboost_honeypotのパス> # ex) /home/user01/boost_honeypot
# chroot下で利用するタイムゾーンの設定
cp -p /etc/localtime ./etc/.

# server.confの編集
vim ./server.conf
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊
[server]
port = <任意のポート>
docroot = <クローンしたboost_honeypotのパス>
user = <chrootで使用するユーザ>
group = <chrootで使用するグループ>
syslogfile = <システムログ出力先 (chroot環境でのパス)>
accesslogfile = <アクセスログ出力先 (chroot環境でのパス)>
accesslogsuffix = <アクセスログの接尾辞 (boost::logの仕様に基づく)>
accesslogrotationsize = <アクセスログのローテーションサイズ (Bytes)>
mrrulesfile = <HTTPマッチングに使用するルールファイル (詳細は./art/mrr_help.txt参照のこと)>
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊

# ハニーポット用ユーザ作成 (chrootで使用するための非特権ユーザ)
sudo groupadd boosthoney
sudo useradd -m -g boosthoney boosthoney

# ファイル読み書き用にファイル所有者変更
sudo chown -R boosthoney:boosthoney etc
sudo chown -R boosthoney:boosthoney art
sudo chown -R boosthoney:boosthoney log

# ビルド実行
make

# ヘルプ
./boosthoney --help
# 起動
sudo ./boosthoney
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
ExecStart = <クローンしたboost_honeypotのパス>/boosthoney
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
![DEMO](./demo.gif)

# License
This software is released under the BSD License, see LICENSE.

[LICENSE](./LICENSE).