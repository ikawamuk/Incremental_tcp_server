use std::env;
use std::process;
fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <port number>", args[0]);
        process::exit(1);
    }
    println!("Hello, world!");
}
