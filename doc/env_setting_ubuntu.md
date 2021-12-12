# Ubuntu環境構築

```bash
# パッケージマネージャの最新化
sudo apt update
sudo apt upgrade

# C++実行環境構築
sudo apt install build-essential

# Boostライブラリインストール
sudo apt install libboost-all-dev

# cmakeインストール
sudo apt install cmake

# ネットワーク設定（任意）
sudo ufw default DENY
sudo ufw allow 80/tcp
sudo ufw allow 8080/tcp
# ※ SSH のアクセスポートも環境に合わせて追加してください。ex)sudo ufw allow 22/tcp

# NAT設定（ポート80のアクセスを8080に転送
sudo vim /etc/ufw/before.rules
# ※ 「*filter」より前に下記の4行を追記する。
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊
*nat
:PREROUTING ACCEPT [0:0]
-A PREROUTING -p tcp --dport 80 -j REDIRECT --to-port 8080
COMMIT
＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊

# ufwのリロード
sudo ufw reload

# ufwの起動設定
sudo ufw enable
```
