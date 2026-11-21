
use std::env;
use std::process;
mod server; use server::Server;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <port number>", args[0]);
        process::exit(1);
    }
    let port: u16 = args[1].parse().expect("invalid port number");
    let server = match Server::new(port) {
        Ok(server) => server,
        Err(e) => {
            eprintln!("Server initialization failed: {e}");
            process::exit(1);
        }
    };
    if let Err(e) = server.run() {
        eprintln!("Server error: {e}");
        process::exit(1);
    }
}
