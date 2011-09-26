MayaOpenNIServer Version 0.2 Alpha

【これはなに？】
KinectをMayaのキャプチャデバイスサーバーとして動作させるプログラムです。

【必要環境】
WindowsXP以降　32bit版
Kinectハードウェア本体
OpenNI
Kinect用ドライバ
NITE

【Kinect/OpenNIのセットアップについて】
既にOpenNIのサンプルが動作している状況が必要です。
UnaNancyOwen氏のこのエントリーあたりが分かりやすく解説されています。

http://kgxpx834.blog58.fc2.com/blog-entry-22.html


【フォルダ構成】
-bin
実行ファイルが入っています。

-MayaScript
Maya用のテスト用スクリプトが入っています。

-src
ソースファイルが入っています

【使用方法】
●サーバー側：
MayaOpenNIServer.exeを実行して下さい。
必要ならポート及びIPアドレスの設定をした後、Start Serverを実行してください。
※デフォルトではポート9999を使用しています。ファイアウォールの設定等を行って下さい。

●Maya側
MELコマンド
defineDataServer -d KinectCapture -s "localhost:9999";
を実行することで、サーバーに接続されます。
Window->Animation Editors->Device Editor
でKinectCaptureが表示されていれば接続できています。

また、サンプルのスクリプトの「Connect」押下でも接続できます。
サンプルのスクリプトで接続する場合、事前にMake Nullsを押下して、ターゲットとなるNullを生成しておいて下さい。

※座標系について：
0.11から座標系をOpenNIの座標系で取り込むように変更しました。
サンプルのMELでは、XとZの位置と回転を反転する様にしています。(47～57行目付近）

【使用ライブラリ等】
・OpenNI
・FLTK2.0

【ライセンス等】
公開にあたって一切の制限は設けません。
OpenNIはLGPL3のライセンス下で公開されていますので、当然、リバースエンジニアリングを禁止しない事項も含みます。


【作成者】
Yasutoshi Mori
Twitter: @yasutoshi_mori
MirageWrks@gmail.com

【更新履歴】
Version0.2
・描画及びOpenNI制御関係のソースをスクラッチから起こし直し
・ジェスチャーへの対応
・ハンド座標への対応
・設定の起動時の復元、終了時の保存
・サンプルコード排除による、LGPLライセンスの撤廃

Version0.11 Alpha
・描画画面サイズを640x480に変更
・GUIにSmoothingパラメータを追加
・パラメータの取得周りをManagerクラスへ
・出力座標系をOpenNIのままに変更

Version0.1 Preview Alpha 2 
・サーバスレッドの制御の見直し
・Managerのスレッド関係の修正
・GUIのレイアウト等見直し
・ダブルバッファが正常に動いてなかったのを修正

(バージョン未割振）Preview Alpha 1.1 
・IPアドレスの変更に対応

(バージョン未割振）Preview Alpha 1 
・初期公開




