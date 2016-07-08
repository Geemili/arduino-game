
extern crate serial;

use std::env;
use std::io;
use std::time::Duration;
use std::process::exit;
use std::thread::sleep;

use std::io::prelude::*;
use serial::prelude::*;

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();
    if args.len() != 1 {
        writeln!(io::stderr(), "Program takes exactly one argument. (The serial port)").unwrap();
        exit(1);
    }

    let mut port = serial::open(&args[0]).unwrap();
    let mut settings = serial::PortSettings::default();
    settings.set_char_size(serial::CharSize::Bits8);
    settings.set_parity(serial::Parity::ParityNone);
    settings.set_stop_bits(serial::StopBits::Stop1);
    if let Err(err) = settings.set_baud_rate(serial::BaudRate::Baud9600) {
        return;
    }

    if let Err(err) = port.configure(&settings) {
        return;
    }

    let mut buf = vec![0; 256];
    let mut message = String::new();
    loop {
        match port.read(&mut buf[..]) {
            Ok(bytes_read) => {
                for i in 0..bytes_read {
                    match buf[i] as char {
                        '\n' if message.trim()=="ready" => {
                            send_level(&mut io::stdin(), &mut port);
                            return;
                        }

                        '\n' => {
                            println!("{}", message=="ready");
                            message.clear();
                        }

                        character => message.push(character)
                    }
                }
            }

            Err(e) => {
                println!("error: {}", e);
            }
        }
        sleep(Duration::from_millis(100));
    }
}

// Reads level and prepares it to be sent
fn read_level(input: &mut Read) -> String {
    let mut buf = String::new();
    input.read_to_string(&mut buf).unwrap();

    let mut result = String::new();
    let mut width = 0u8;
    let mut height = 0u8;
    for line in buf.trim().lines() {
        result.push_str(line);
        height += 1;
        width = if line.len() as u8 > width {line.len() as u8} else {width};
    }
    format!("{}{}{}", width as char, height as char, result)
}

fn send_level(input: &mut Read, output: &mut Write) {
    let level = read_level(input);
    write!(output, "{}", level).unwrap();
}
