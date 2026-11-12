mod storage;
mod structures;
mod database;

use database::Database;
use std::env;
use std::io::{self, Write};
use std::path::PathBuf;

fn run() -> Result<(), String> {
    let mut file = PathBuf::from("file.data");
    let mut query: Option<String> = None;
    let mut args = env::args().skip(1);

    while let Some(arg) = args.next() {
        match arg.as_str() {
            "--file" => file = PathBuf::from(args.next().ok_or("не указан файл")?),
            "--query" => query = Some(args.next().ok_or("не указана команда")?),
            _ => return Err(format!("неизвестный параметр: {arg}")),
        }
    }

    let mut db = Database::open(file)?;
    if let Some(line) = query {
        let answer = db.command(&line)?;
        if !answer.is_empty() { println!("{answer}"); }
        return Ok(());
    }

    println!("1 Массив | 2 Односвязный | 3 Двусвязный | 4 Стек | 5 Очередь | 0 Выход");
    println!("Команды можно вводить напрямую. HELP - справка.");

    let mut line = String::new();
    loop {
        print!("> ");
        io::stdout().flush().map_err(|e| e.to_string())?;
        line.clear();
        if io::stdin().read_line(&mut line).map_err(|e| e.to_string())? == 0 {
            break;
        }
        let text = line.trim();
        match text {
            "0" | "EXIT" => break,
            "1" => { println!("{}", db.summary('M')); continue; }
            "2" => { println!("{}", db.summary('F')); continue; }
            "3" => { println!("{}", db.summary('L')); continue; }
            "4" => { println!("{}", db.summary('S')); continue; }
            "5" => { println!("{}", db.summary('Q')); continue; }
            "BACK" => continue,
            _ => {}
        }
        match db.command(text) {
            Ok(answer) if answer == "EXIT" => break,
            Ok(answer) if answer == "BACK" => continue,
            Ok(answer) if !answer.is_empty() => println!("{answer}"),
            Err(error) => eprintln!("Ошибка: {error}"),
            _ => {}
        }
    }
    Ok(())
}

fn main() {
    if let Err(error) = run() {
        eprintln!("Ошибка: {error}");
        std::process::exit(1);
    }
}
