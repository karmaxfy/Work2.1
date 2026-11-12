use std::collections::BTreeMap;
use std::path::PathBuf;

use crate::storage::{Data, format, load, save};
use crate::structures::{Array, DoublyList, Queue, SinglyList, Stack};

pub struct Database {
    file: PathBuf,
    arrays: BTreeMap<String, Array>,
    singles: BTreeMap<String, SinglyList>,
    doubles: BTreeMap<String, DoublyList>,
    stacks: BTreeMap<String, Stack>,
    queues: BTreeMap<String, Queue>,
    trees: Data,
}

fn missing() -> String {
    "контейнер не найден".into()
}

fn index(text: &str) -> Result<usize, String> {
    text.parse::<usize>().map_err(|_| "неверный индекс".into())
}

fn find<'a, T>(map: &'a mut BTreeMap<String, T>, name: &str) -> Result<&'a mut T, String> {
    map.get_mut(name).ok_or_else(missing)
}

fn create<T: Default>(map: &mut BTreeMap<String, T>, name: &str) -> (String, bool) {
    use std::collections::btree_map::Entry;
    match map.entry(name.to_owned()) {
        Entry::Vacant(entry) => {
            entry.insert(T::default());
            ("OK".into(), true)
        }
        Entry::Occupied(_) => ("EXISTS".into(), false),
    }
}

fn yes(value: bool) -> String {
    match value {
        true => "TRUE".into(),
        false => "FALSE".into(),
    }
}

fn updated(success: bool) -> (String, bool) {
    match success {
        true => ("OK".into(), true),
        false => ("NOT FOUND".into(), false),
    }
}

fn expected(cmd: &str) -> Option<usize> {
    match cmd {
        "HELP" | "BACK" | "EXIT" => Some(1),
        "PRINT" => Some(3),
        "MCREATE" | "FCREATE" | "LCREATE" | "SCREATE" | "QCREATE"
        | "MLEN" | "FDELHEAD" | "FDELTAIL" | "LDELHEAD" | "LDELTAIL"
        | "LREVERSE" | "SPOP" | "SGET" | "QPOP" | "QGET"
        | "FGETHEAD" | "FGETTAIL" | "LGETHEAD" | "LGETTAIL" => Some(2),
        "MPUSH" | "MGET" | "MDEL" | "FPUSH" | "FPUSHFRONT"
        | "FDEL" | "FGET" | "FFIND" | "LPUSH" | "LPUSHFRONT"
        | "LDEL" | "LGET" | "LFIND" | "SPUSH" | "QPUSH"
        | "FDELBEFORE" | "FDELAFTER" | "LDELBEFORE" | "LDELAFTER" => Some(3),
        "MINSERT" | "MSET" | "FAFTER" | "FBEFORE"
        | "LAFTER" | "LBEFORE" => Some(4),
        _ => None,
    }
}

const HELP: &str = "M: MCREATE MPUSH MINSERT MGET MSET MDEL MLEN\n\
F/L: FCREATE LCREATE FPUSH LPUSH FPUSHFRONT LPUSHFRONT FAFTER LAFTER FBEFORE LBEFORE FDEL LDEL FDELHEAD LDELHEAD FDELTAIL LDELTAIL FGET LGET FFIND LFIND LREVERSE FDELBEFORE FDELAFTER LDELBEFORE LDELAFTER FGETHEAD FGETTAIL LGETHEAD LGETTAIL\n\
S: SCREATE SPUSH SPOP SGET\n\
Q: QCREATE QPUSH QPOP QGET\n\
PRINT M|F|L|S|Q name; BACK; EXIT";

impl Database {
    pub fn open(file: PathBuf) -> Result<Self, String> {
        let mut db = Self {
            file: file.clone(),
            arrays: BTreeMap::new(),
            singles: BTreeMap::new(),
            doubles: BTreeMap::new(),
            stacks: BTreeMap::new(),
            queues: BTreeMap::new(),
            trees: Data::new(),
        };
        for (key, values) in load(&file)? {
            let (kind, name) = key.split_once(':').ok_or("неверное имя контейнера")?;
            match kind {
                "M" => {
                    let mut item = Array::default();
                    for value in values { item.push(value); }
                    db.arrays.insert(name.into(), item);
                }
                "F" => {
                    let mut item = SinglyList::default();
                    for value in values { item.push_back(value); }
                    db.singles.insert(name.into(), item);
                }
                "L" => {
                    let mut item = DoublyList::default();
                    for value in values { item.push_back(value); }
                    db.doubles.insert(name.into(), item);
                }
                "S" => {
                    let mut item = Stack::default();
                    for value in values { item.push(value); }
                    db.stacks.insert(name.into(), item);
                }
                "Q" => {
                    let mut item = Queue::default();
                    for value in values { item.push(value); }
                    db.queues.insert(name.into(), item);
                }
                "T" => { db.trees.insert(key.clone(), values); }
                _ => return Err("неверный тип контейнера".into()),
            }
        }
        Ok(db)
    }

    fn snapshot(&self) -> Data {
        let mut data = self.trees.clone();
        for (name, item) in &self.arrays {
            data.insert(format!("M:{name}"), item.values());
        }
        for (name, item) in &self.singles {
            data.insert(format!("F:{name}"), item.values());
        }
        for (name, item) in &self.doubles {
            data.insert(format!("L:{name}"), item.values());
        }
        for (name, item) in &self.stacks {
            let mut values = item.values();
            values.reverse();
            data.insert(format!("S:{name}"), values);
        }
        for (name, item) in &self.queues {
            data.insert(format!("Q:{name}"), item.values());
        }
        data
    }

    pub fn summary(&self, kind: char) -> String {
        let data = self.snapshot();
        let mut lines = Vec::new();
        for (key, values) in data {
            if key.starts_with(kind) {
                lines.push(format!("{}: {}", &key[2..], format(&values)));
            }
        }
        match lines.is_empty() {
            true => "(none)".into(),
            false => lines.join("\n"),
        }
    }

    fn print(&self, kind: &str, name: &str) -> Result<String, String> {
        let values = match kind {
            "M" => self.arrays.get(name).ok_or_else(missing)?.values(),
            "F" => self.singles.get(name).ok_or_else(missing)?.values(),
            "L" => self.doubles.get(name).ok_or_else(missing)?.values(),
            "S" => self.stacks.get(name).ok_or_else(missing)?.values(),
            "Q" => self.queues.get(name).ok_or_else(missing)?.values(),
            "T" => return Err("АВЛ-дерево выполняется только на C++".into()),
            _ => return Err("неизвестный тип".into()),
        };
        Ok(format(&values))
    }

    // Отдельная функция на каждый тип структуры вместо одного огромного match.
    fn array(&mut self, cmd: &str, a: &[&str]) -> Result<(String, bool), String> {
        let item = match cmd {
            "MPUSH" => self.arrays.entry(a[1].into()).or_default(),
            _ => find(&mut self.arrays, a[1])?,
        };
        match cmd {
            "MPUSH" => { item.push(a[2].into()); Ok(("OK".into(), true)) }
            "MINSERT" => { item.insert(index(a[2])?, a[3].into())?; Ok(("OK".into(), true)) }
            "MGET" => Ok((item.get(index(a[2])?)?, false)),
            "MSET" => { item.set(index(a[2])?, a[3].into())?; Ok(("OK".into(), true)) }
            "MDEL" => Ok((item.erase(index(a[2])?)?, true)),
            "MLEN" => Ok((item.len().to_string(), false)),
            _ => Err("неизвестная команда массива".into()),
        }
    }

    fn single(&mut self, cmd: &str, a: &[&str]) -> Result<(String, bool), String> {
        let item = match cmd {
            "FPUSH" | "FPUSHFRONT" => self.singles.entry(a[1].into()).or_default(),
            _ => find(&mut self.singles, a[1])?,
        };
        match cmd {
            "FPUSH" => { item.push_back(a[2].into()); Ok(("OK".into(), true)) }
            "FPUSHFRONT" => { item.push_front(a[2].into()); Ok(("OK".into(), true)) }
            "FAFTER" => Ok(updated(item.after(a[2], a[3].into()))),
            "FBEFORE" => Ok(updated(item.before(a[2], a[3].into()))),
            "FDEL" => Ok(updated(item.remove(a[2]))),
            "FDELHEAD" => Ok((item.pop_front()?, true)),
            "FDELTAIL" => Ok((item.pop_back()?, true)),
            "FGET" => Ok((item.get(index(a[2])?)?, false)),
            "FFIND" => Ok((yes(item.contains(a[2])), false)),
            "FDELBEFORE" => Ok(updated(item.remove_before(a[2]))),
            "FDELAFTER" => Ok(updated(item.remove_after(a[2]))),
            "FGETHEAD" => Ok((item.front()?, false)),
            "FGETTAIL" => Ok((item.back()?, false)),
            _ => Err("неизвестная команда списка".into()),
        }
    }

    fn double(&mut self, cmd: &str, a: &[&str]) -> Result<(String, bool), String> {
        let item = match cmd {
            "LPUSH" | "LPUSHFRONT" => self.doubles.entry(a[1].into()).or_default(),
            _ => find(&mut self.doubles, a[1])?,
        };
        match cmd {
            "LPUSH" => { item.push_back(a[2].into()); Ok(("OK".into(), true)) }
            "LPUSHFRONT" => { item.push_front(a[2].into()); Ok(("OK".into(), true)) }
            "LAFTER" => Ok(updated(item.after(a[2], a[3].into()))),
            "LBEFORE" => Ok(updated(item.before(a[2], a[3].into()))),
            "LDEL" => Ok(updated(item.remove(a[2]))),
            "LDELHEAD" => Ok((item.pop_front()?, true)),
            "LDELTAIL" => Ok((item.pop_back()?, true)),
            "LGET" => Ok((item.get(index(a[2])?)?, false)),
            "LFIND" => Ok((yes(item.contains(a[2])), false)),
            "LREVERSE" => Ok((format(&item.reverse()), false)),
            "LDELBEFORE" => Ok(updated(item.remove_before(a[2]))),
            "LDELAFTER" => Ok(updated(item.remove_after(a[2]))),
            "LGETHEAD" => Ok((item.front()?, false)),
            "LGETTAIL" => Ok((item.back()?, false)),
            _ => Err("неизвестная команда списка".into()),
        }
    }

    fn stack(&mut self, cmd: &str, a: &[&str]) -> Result<(String, bool), String> {
        let item = match cmd {
            "SPUSH" => self.stacks.entry(a[1].into()).or_default(),
            _ => find(&mut self.stacks, a[1])?,
        };
        match cmd {
            "SPUSH" => { item.push(a[2].into()); Ok(("OK".into(), true)) }
            "SPOP" => Ok((item.pop()?, true)),
            "SGET" => Ok((item.get()?, false)),
            _ => Err("неизвестная команда стека".into()),
        }
    }

    fn queue(&mut self, cmd: &str, a: &[&str]) -> Result<(String, bool), String> {
        let item = match cmd {
            "QPUSH" => self.queues.entry(a[1].into()).or_default(),
            _ => find(&mut self.queues, a[1])?,
        };
        match cmd {
            "QPUSH" => { item.push(a[2].into()); Ok(("OK".into(), true)) }
            "QPOP" => Ok((item.pop()?, true)),
            "QGET" => Ok((item.get()?, false)),
            _ => Err("неизвестная команда очереди".into()),
        }
    }

    pub fn command(&mut self, line: &str) -> Result<String, String> {
        let args: Vec<&str> = line.split_whitespace().collect();
        if args.is_empty() {
            return Ok(String::new());
        }
        let cmd = args[0].to_ascii_uppercase();
        let count = expected(&cmd).ok_or("неизвестная команда (HELP - справка)")?;
        if args.len() != count {
            return Err("неверное число аргументов".into());
        }

        match cmd.as_str() {
            "HELP" => return Ok(HELP.into()),
            "BACK" => return Ok("BACK".into()),
            "EXIT" => return Ok("EXIT".into()),
            "PRINT" => return self.print(&args[1].to_ascii_uppercase(), args[2]),
            _ => {}
        }
        let name = args[1];
        if name.contains(':') || name.is_empty() {
            return Err("неверное имя контейнера".into());
        }

        let created = match cmd.as_str() {
            "MCREATE" => Some(create(&mut self.arrays, name)),
            "FCREATE" => Some(create(&mut self.singles, name)),
            "LCREATE" => Some(create(&mut self.doubles, name)),
            "SCREATE" => Some(create(&mut self.stacks, name)),
            "QCREATE" => Some(create(&mut self.queues, name)),
            _ => None,
        };
        if let Some((message, changed)) = created {
            if changed { save(&self.file, &self.snapshot())?; }
            return Ok(message);
        }

        let (message, changed) = match cmd.chars().next().unwrap() {
            'M' => self.array(&cmd, &args)?,
            'F' => self.single(&cmd, &args)?,
            'L' => self.double(&cmd, &args)?,
            'S' => self.stack(&cmd, &args)?,
            'Q' => self.queue(&cmd, &args)?,
            _ => return Err("неизвестная команда".into()),
        };
        if changed {
            save(&self.file, &self.snapshot())?;
        }
        Ok(message)
    }
}
