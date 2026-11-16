# Incremental TCP Server

最小限のTCPサーバーから、段階的にReactorパターンの実装を目指すC++の学習用プロジェクトです。

## 現在の状態

- ノンブロッキングTCP通信と複数クライアントの管理
- 接続ごとの受信・送信バッファ、部分送信と受信待ち／EOFの区別
- 通信処理とプロトコル解析の分離（解析結果・消費バイト数を返す `RequestProcessor`）
- `ServerConfig` による依存の注入と、`IPoller` 経由のイベント処理

**現在はイベント待機の実装途中です。** `IPoller` の具象実装がなく、`main.cpp` もポーラーを渡していないため、ソケット生成・待ち受けに成功しても `Error: Poller is null` で終了します。次は `poll` / `epoll` などの実装・接続と、切断・エラー時のイベント処理を整備します。

## ビルド・起動

Linux、C++20対応コンパイラ、Makeが必要です。MakefileではAddressSanitizer / UndefinedBehaviorSanitizerを有効にしています。

```sh
make
./reactor 8080
```

ポーラー接続後は `127.0.0.1:<指定ポート>` で待ち受け、Ctrl+CまたはSIGTERMで終了する構成です。

## コマンド

ポーラー接続後の接続例：`nc 127.0.0.1 8080`。コマンドは改行ではなく `;` で区切ります。

| 入力 | 応答 |
| --- | --- |
| `HELLO Itsuki;` | `hello, Mr.Itsuki!` |
| `HELLO;` | `hello, Sir.` |
| `BYE;` | `Good bye!` |

未対応のコマンドには使い方を返します。`BYE;` は応答のみで、接続は終了しません。
