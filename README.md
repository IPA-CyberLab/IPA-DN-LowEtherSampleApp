IPA-DN-LowEtherSampleApp アプリケーション
Linux 上での低レベル Ethernet フレームの送受信のサンプル

■ 準備
Linux 上で gcc 等のビルドツールおよび必要なライブラリを以下のようにインストールしてください。

・ CentOS の場合
# yum -y groupinstall "Development Tools"
# yum -y install readline-devel ncurses-devel openssl-devel git

・ Ubuntu, Debian の場合
# apt-get -y install build-essential libncurses5-dev libreadline-dev libssl-dev openssl git

■ サンプルソースコードの取得
Git から以下のとおり取得してください。

# git clone --recursive https://github.com/IPA-CyberLab/IPA-DN-LowEtherSampleApp.git

■ サンプルソースコードのビルド
Git で取得したディレクトリに移動します。
# cd IPA-DN-LowEtherSampleApp

ビルドします。
# make

bin/lowether という実行可能ファイルが生成されたことを確認します。
# ls -la bin/lowether

■ サンプルの実行
まず引数を付けずに起動してみます。

# bin/lowether
IPA-DN-LowEtherSampleApp by dnobori
--- List of available Ethernet adapters ---
NIC #0: eth0
  description: eth0

上記のように、システム上で利用可能な Ethernet アダプタの名前一覧が表示されます。上記の例では、「eth0」という 1 枚の NIC が存在しています。

次に引数を付けて実行します。
# bin/lowether eth0

これでプログラムの動作が開始されます。

Ethernet フレームを受信したら、

[Tick: 5340] [RECV 60 bytes] 00 11 22 33 ... (16 進ダンプ)

のように表示されます。Tick の値はプログラム起動時からの 1ms 単位の経過時間です。

1 秒間に 1 回、サンプルの ARP フレームを送信します。
フレームを送信したときは、以下のように表示されます。

[Tick: 2053] [SEND 42 bytes] FF FF FF FF ... (16 進ダンプ)

■ このプログラムの解説
main.c にすべての重要な処理が記載されています。

main() 関数は、コマンドライン引数を検査し、コマンドライン引数が指定されていない場合は NIC の一覧を列挙します。
コマンドライン引数が指定されている場合は指定された文字列の NIC デバイスを開き、送受信のループスレッドである main_loop_thread() 関数を新しいスレッドで開始します。

「// SAMPLE: send an Ethernet frame with regular interval (1000ms)」と記載されている部分のコードにより、1 秒に 1 回、ARP パケットを構築し、Ethernet ヘッダを付加して NIC 経由で送信をしています。

「// SAMPLE: Receive Ethernet frames」と記載されている部分のコードにより、新たなフレームが NIC に届いたら、その内容を表示しています。この際、TCP ヘッダを検査し、送受信のいずれかのポート番号が 22 (SSH) である場合は表示をしないようにしています。その理由は、大抵の場合このプログラムは SSH ターミナル経由で実行しますが、SSH パケットが画面に出力されようとするとその出力を SSH で送信するために新たなパケットが生成され、この無限の繰り返しが発生してしまうため、これを避けるために SSH パケットは表示から除外するようにしておるのです。

■ サンプルで送信する ARP パケットについて
デフォルトでは、src ip: 192.168.0.1、 dest ip: 192.168.0.2 の ARP リクエストを送信するようになっています。

実験を行なうネットワークの実情に合わせてください。

■ VLAN タグ付きフレームの送受信
自分で VLAN タグを付けることにより、物理 NIC から、IEEE802.1Q VLAN タグ付きフレームを送受信することができます。

■ VM 上で動作させる場合の注意事項
送信元 MAC アドレスを適当に乱数で生成しているため、VM のホスト上の設定で、
・ Promiscuous モードを許可する
・ 発信元 MAC アドレスの偽装を許可する
を行なう必要があります。
Hyper-V、VMware ESXi とも上記のような設定が可能ですので、必ず設定をして利用してください。
そのようにしなければ、パケットの送受信がうまくできません。

■ 注意事項
本プログラムは、実験用のネットワークでのみ動作させてください。
本プログラムを不用意に動作させたり、色々書き換えたりすると、一般的なネットワークを麻痺させることができます。現用ネットワークでは使用しないでください。
また、生の TCP/IP パケットをインターネットに対して自由に組立てて送受信できますが、インターネットに対して有害なパケットは送信しないようにしてください。(送信をすると問題になります。)

2018-07-31 登

