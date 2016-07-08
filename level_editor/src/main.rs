
extern crate serial;

use std::env;
use std::io;
use std::process::exit;

use std::io::prelude::*;

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();
    if args.len() != 1 {
        writeln!(io::stderr(), "Program takes exactly one argument. (The serial port)").unwrap();
        exit(1);
    }
    let mut port = serial::open(&args[0]).unwrap();
    send_level(&mut io::stdin(), &mut port);
}

fn send_level(input: &mut Read, output: &mut Write) {
    let mut level_data = vec![];
    match input.read_to_end(&mut level_data) {
        Ok(_) => {
            output.write(level_data.as_slice()).unwrap();
        },
        _ => {
            writeln!(io::stderr(), "Could not read standard input.").unwrap();
            exit(2);
        }
    }
}
