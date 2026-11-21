use std::io;
use std::net::TcpListener;
use std::io::Read;

pub struct Server {
    listener: TcpListener,
}

impl Server {
    pub fn new(port: u16) -> io::Result<Self> {
        let address = format!("127.0.0.1:{port}");
        let listener = TcpListener::bind(address)?;
        Ok(Self { listener })
    }

    pub fn run(&self) -> io::Result<()> {
        let (mut stream, _) = self.listener.accept()?;
        let mut buf = [0u8; 1024];
        let n: usize = stream.read(&mut buf)?;
        let text = String::from_utf8_lossy(&buf[..n]);
        println!("{text}");
        Ok(())
    }
}
