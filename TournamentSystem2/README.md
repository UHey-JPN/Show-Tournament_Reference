#Tournament System 2

ver.0.80

by 黒岩　亮 (Twitter: @kakira9618)

## 概要
[ChibaRobo_Server](https://github.com/UHey-JPN/ChibaRobo_Server) 及び [ChibaRobo_Console](https://github.com/UHey-JPN/ChibaRobo_Console) と連動して動くSiv3D製のトーナメントシステムです。

## 起動方法
1. 設定を行います。設定方法は項『設定』を参照してください。
2. ChibaRobo_Serverを起動します。サーバーのIP、TCP_data_portなどを確認し、再度設定します。
3. 設定後、TournamentSystem2.exe をダブルクリックして起動します。ファイアウォール等の警告が出た場合は許可します。
4. 起動するまで待ちます。平均起動時間は30秒ほどです（読み込む写真のデータ量によります）。
5. 以後の操作は、ChibaRobo_Console　より行います。（設定のuse_networkがONの場合。OFFの場合はキーボードによる操作が行えます）

## 操作方法
[ChibaRobo_Console側の操作]

1. ChibaRobo_ConsoleがChibaRobo_Serverと接続できているかをチェックします（Top）
2. Operationをクリックします
3. 最初は`Opening`ボタンを押します。このボタンを押すことで、オープニング動画の再生が行われると同時に、以後の操作が可能になります。
4. 動画の再生が終わったら（もしくは途中で中断したい場合）、`Home`ボタンを押します。
5. トーナメント画面が現れます
6. `New Game` -> (`Left Interview` <-> `Right Interview`) -> `Game Start` -> `Result` -> どちらかの`Win` -> (`Left Interview` <-> `Right Interview`) -> `Home` のような順番でボタンを押します（詳しくはChibaRobo_Console側の操作に従ってください）。Consoleと連携してトーナメント側の画面が動きます。
7. Tournament System　2を終了させる場合は、Tournament System　2が起動しているPC上で`Alt + F4`を押します。

[手動モードの場合]

1. 全てキーボードで操作します。
2. 起動が終了したら、`Enter`キーを押すとオープニング動画が流れます
3. `Enter`キーでトーナメントメイン画面に進みます。
4. その後も基本的に`Enter`キーで進めますが、勝者チームを選択する場合、左右キーを押します。
5. Tournament System　2を終了させる場合は、Tournament System　2が起動しているPC上で`Alt + F4`を押します。

## 設定
1. まず、写真を`./DB/img/` に置きます。（このバージョンではネットワーク利用による写真のロードに対応していません。）
	1. ファイル形式: jpgまたはpng
	2. 大きさ: 512*512[px]推奨（正方形でなくても動きますが引き伸ばされます。写真が大きすぎると起動時間に影響します）。
	3. ファイルの名前: 英数字アンダーバー推奨（日本語は一応可能ですが、特殊文字は不可）
2. `./setting.txt`を開き、編集します。
	1. 1行目[console_visible]: コンソール画面を表示するかを設定します。（1=表示する、0=表示しない）
	2. 2行目[full_screen]: フルスクリーンモードであるかを設定します。（1=フルスクリーン、0=ウィンドウ）
	3. 3行目[window_width]: ウィンドウの幅[px]を指定します。フルスクリーンモードの場合、横解像度になりますので、GPUにてサポートされている数値に設定してください
	4. 4行目[window_height]: ウィンドウの高さ[px]を指定します。
	5. 5行目[use_network]: ネットワークを使うかどうかを指定します。（1=使う(ChibaRobo_Server, ChibaRobo_Console連携モード)、0=使わない（手動操作モード））
	6. 6行目[server_ip_address]: ChibaRobo_ServerのIPアドレスを指定します。
	7. 7行目[server_tcp_data_port]: ChibaRobo_ServerのTCPデータポートを指定します。
	8. 8行目[team_desc_fontsize]: チーム名のフォントの大きさ[px]を指定します。
	9. 9行目[robot_desc_fontsize]: ロボット名のフォントの大きさ[px]を指定します。
	10. 10行目[opening_video_path]: オープニング動画のパスを指定します。exeと同じ階層に置いてください。（例: 同じ階層にopening.wmvを置いた場合単に`opening.wmv`とする）
	11. 11行目[xml_write_ok]: ChibaRobo_Serverから受け取ったTCPデータ(robot, team, tournament)をxmlファイルに書き込むかを指定します。これがONでないと、ChibaRobo_Server側のTCPデータが反映されないので注意。(1=書き込む、0=書き込まない)
	12. 12行目[winner_effect]WINNERエフェクトを表示するかどうか（1=表示する、0=表示しない）
	13. 13行目[winner_desc_fontsize]: 勝者チーム画面に常時表示されるWINNER文字の大きさ（WINNERエフェクトの設定とは独立です）

## 実装されていない機能
- KeepAlive機能
- ネットワーク利用による写真のロード

## 未確認機能

## 更新履歴
- ver.0.10 (2016年hoge月fuga日) 初版完成
- ver.0.50 (2017年11月2日)　nc.exeを使わないプログラムに修正
- ver.0.60 (2017年11月3日)　コンソールにおける文字化けが起こらないように修正・オープニング動画（仮）のサイズを軽く。
- ver.0.70 (2017年11月3日) 勝者判定を追加
- ver.0.80 (2017年11月3日) オープニングをループしないように修正・L_team/R_team表示中にnewgameへshowstateが移ったときの処理を追加