# Architecture

`main` は `TCPTransport`、`EpollPoller`、JSON RPC 用の `RequestProcessor`、停止条件を `ServerConfig` に渡します。`Server` は待ち受けソケットと接続を所有し、`Reactor` はポーラーと FD ごとのイベントハンドラを所有します。

## モジュール構成

```mermaid
classDiagram
    class Server {
        +run()
        -accept_client()
        -handle_listening_event(Event&)
        -handle_client_event(Event&)
        -handle_client_state(Client&)
        -remove_client(Client&)
    }
    class Reactor {
        +run(ShouldStop)
        +register_handler(target, interests, handler)
        +modify_handler(target, interests)
        +unregister_handler(target)
        -handle_event(Event&)
    }
    class IPoller {
        <<interface>>
        +register_target(target, events)
        +modify_target(target, events)
        +unregister_target(target)
        +poll(timeout_ms) vector~Event~
    }
    class IPollable {
        <<interface>>
        +handle() Handle
    }
    class Event {
        +Handle handle
        +uint32_t event_flag
    }
    class ITransport {
        <<interface>>
        +accept() unique_ptr~IByteStream~
    }
    class IByteStream {
        <<interface>>
        +read(buffer, size) optional~size_t~
        +write(buffer, size) optional~size_t~
    }
    class Client {
        +try_read()
        +try_write()
        +need_read() bool
        +need_write() bool
        +registered_interests() unsigned int
    }
    class RequestProcessor {
        +process(input) ProcessResult
    }

    Server ..> ServerConfig
    Server *-- Reactor
    Server *-- ITransport
    Server "1" *-- "0..*" Client
    Server *-- RequestProcessor : 接続に渡す処理
    Reactor *-- IPoller
    Reactor ..> Event : FD でハンドラを選ぶ
    IPoller ..> IPollable
    IPoller ..> Event
    EpollPoller ..|> IPoller
    ITransport --|> IPollable
    IByteStream --|> IPollable
    TCPTransport ..|> ITransport
    TCPByteStream ..|> IByteStream
    ITransport ..> IByteStream : accept
    Client *-- IByteStream
    Client *-- RequestProcessor
    RequestProcessor ..> ProcessResult
```

- `Handle` は `int` の別名で、TCP ではソケットの FD です。`IPoller` は読み取り可能・書き込み可能・エラーを `Event::event_flag` で返します。
- `Reactor` は FD と `EventHandler`（`std::function<void(Event&)>`）の対応を保持します。監視登録が失敗した場合は追加したハンドラを取り消し、監視解除時は対応するハンドラも削除します。
- `Client` は接続ごとのバイトストリーム、受信・送信バッファ、読み取り状態、登録済みの監視マスクを持ちます。
- `RequestProcessor` は注入された `Parser` を呼びます。`ProcessResult` は `Processed`（応答と消費バイト数）、`NeedMore`（追加受信待ち）、`Discard`（破棄するバイト数）のいずれかです。

## イベントの流れ

1. `Server::run()` が待ち受け FD とハンドラを `Reactor` に登録し、停止条件を渡してイベントループを起動します。
2. `Reactor` が `IPoller::poll()` で待機し、通知された FD に対応するハンドラを呼びます。
3. 待ち受け FD が読み取り可能になると、`Server` が接続を受け付け、新しい `Client` と接続用ハンドラを登録します。
4. 接続用ハンドラは通知に応じて `Client` の読み書きを行います。受信したバイト列は `RequestProcessor` に渡され、応答があれば送信バッファに追加されます。
5. `Server` は `Client` の状態から次の監視マスクを決め、変更があれば `Reactor` に反映を依頼します。接続を閉じる場合は `Reactor` で監視とハンドラを解除してから `Client` を破棄します。
