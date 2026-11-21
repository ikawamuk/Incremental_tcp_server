use std::env;
use std::process;
use std::net::TcpListener;
use std::io::Read;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <port number>", args[0]);
        process::exit(1);
    }
    let listener: TcpListener = TcpListener::bind("127.0.0.1:8080")
       .expect("Failed to bind");
    let (mut stream, addr) = listener.accept()
        .expect("accept failed");
    println!("client accept: {}", addr);
    let mut buf = [0u8; 1024];
    let n: usize = stream.read(&mut buf)
        .expect("read failed");
    let text = std::str::from_utf8(&buf[..n])
        .expect("invalid UTF-8");
    println!("{}", text);
}
