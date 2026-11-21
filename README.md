# Incremental TCP Server

ノンブロッキング TCP と epoll で複数のクライアントを扱う、C++ の Reactor パターン学習プロジェクトです。独自の JSON RPC 風プロトコルで四則演算を提供します。JSON の解析には同梱の自作ライブラリ rosetta_json を使います。

## ビルド・起動

Linux、C++20 対応の C++ コンパイラと標準ライブラリ、C コンパイラ、Make が必要です。Makefile ではサーバーのビルドに AddressSanitizer と UndefinedBehaviorSanitizer を有効にしています。

```sh
make
./reactor 8080
```

ポート番号は 1〜65535 の整数です。サーバーは IPv4 ループバックの `127.0.0.1:<指定ポート>` で待ち受けます。Ctrl+C または SIGTERM で終了します。

## 通信仕様

要求と応答はともに `RS (0x1E) + JSON + LF (0x0A)` の形式です。JSON 本文は１行で送り、要求フレームは RS と LF を含めて最大 64 KiB です。上限を超えた場合は接続を閉じます。

要求にはメソッド名 `method` と、有限の JSON 数値２個を含む配列 `params` を指定します。演算には `double` を使います。

| メソッド | 計算 |
| --- | --- |
| `add` | 第１引数 + 第２引数 |
| `sub` | 第１引数 − 第２引数 |
| `mul` | 第１引数 × 第２引数 |
| `div` | 第１引数 ÷ 第２引数 |

JSON 本文の例です。

- 要求 `{"method":"add","params":[2,3]}`
- 成功応答 `{"result":5}`
- ゼロ除算の応答 `{"error":"Division by zero"}`
- 不正な要求への応答 `{"error":"Invalid Request"}`

１接続で複数の要求を送れます。応答は要求を処理した順に返ります。

## 疎通確認

サーバーをポート 8080 で起動し、別の端末で Python 3 を使って確認できます。

```python
import json
import socket

with socket.create_connection(("127.0.0.1", 8080), timeout=3) as sock:
    sock.sendall(b'\x1e{"method":"add","params":[2,3]}\n')
    with sock.makefile("rb") as response:
        frame = response.readline()
        if not frame.startswith(b"\x1e") or not frame.endswith(b"\n"):
            raise ValueError(f"Invalid response frame: {frame!r}")
        print(json.loads(frame[1:-1]))  # {'result': 5}
```

## 設計

`ServerConfig` でトランスポート、ポーラー、リクエスト処理、停止条件を注入します。

- `Server` は待ち受けソケットと接続を管理し、それぞれの FD に対するイベントハンドラを `Reactor` に登録します。
- `Reactor` は `IPoller` を使ってイベントを待ち、通知された FD のハンドラを呼びます。FD の監視とハンドラの登録・解除も担当します。Linux でのポーラー実装は `EpollPoller` です。
- `Client` は接続の読み書きを担当し、受信データを `RequestProcessor` に渡します。現在は JSON RPC のフレーム解析と四則演算を行う処理を注入しています。

受信・送信バッファは接続ごとに持ち、部分受信と部分送信を扱います。送信待ちが 48 KiB 以上になると読み取りを一時停止し、16 KiB 以下まで減ると、受信済みの要求から処理を再開します。送信待ちの上限は 64 KiB で、応答の追加が上限を超える場合は接続を閉じます。

モジュール構成は [設計図](doc/Architecture.md) を参照してください。
