use std::collections::BTreeMap;
use std::fs;
use std::path::Path;

pub type Data = BTreeMap<String, Vec<String>>;

// Один текстовый файл для обеих программ: тип, имя и значения через табуляцию.
fn escape(text: &str) -> String {
    let mut result = String::new();
    for c in text.chars() {
        match c {
            '\\' => result.push_str("\\\\"),
            '\t' => result.push_str("\\t"),
            '\n' => result.push_str("\\n"),
            '\r' => result.push_str("\\r"),
            _ => result.push(c),
        }
    }
    result
}

fn unescape(text: &str) -> Result<String, String> {
    let mut result = String::new();
    let mut chars = text.chars();
    while let Some(c) = chars.next() {
        if c != '\\' {
            result.push(c);
            continue;
        }
        let next = chars.next().ok_or("ошибка экранирования")?;
        match next {
            '\\' => result.push('\\'),
            't' => result.push('\t'),
            'n' => result.push('\n'),
            'r' => result.push('\r'),
            _ => return Err("ошибка экранирования".into()),
        }
    }
    Ok(result)
}

pub fn load(file: &Path) -> Result<Data, String> {
    if !file.exists() {
        return Ok(Data::new());
    }
    let content = fs::read_to_string(file).map_err(|e| e.to_string())?;
    let mut data = Data::new();

    for line in content.lines() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let fields: Vec<String> = line.split('\t').map(unescape).collect::<Result<_, _>>()?;
        if fields.len() < 2 || fields[0].len() != 1 || fields[1].is_empty() {
            return Err("неправильный формат файла".into());
        }
        if !"MFLSQT".contains(fields[0].as_str()) || fields[1].contains(':') {
            return Err("неправильный тип или имя контейнера".into());
        }
        let key = format!("{}:{}", fields[0], fields[1]);
        if data.insert(key, fields[2..].to_vec()).is_some() {
            return Err("повтор имени контейнера".into());
        }
    }
    Ok(data)
}

pub fn save(file: &Path, data: &Data) -> Result<(), String> {
    if let Some(parent) = file.parent() {
        if !parent.as_os_str().is_empty() {
            fs::create_dir_all(parent).map_err(|e| e.to_string())?;
        }
    }
    let mut content = String::from("# LAB2-1\n");
    for (key, values) in data {
        let (kind, name) = key.split_once(':').ok_or("неправильное имя")?;
        content.push_str(kind);
        content.push('\t');
        content.push_str(&escape(name));
        for value in values {
            content.push('\t');
            content.push_str(&escape(value));
        }
        content.push('\n');
    }
    let mut temp = file.as_os_str().to_os_string();
    temp.push(".tmp");
    let temp = std::path::PathBuf::from(temp);
    fs::write(&temp, content).map_err(|e| e.to_string())?;
    fs::rename(&temp, file).map_err(|e| e.to_string())
}

pub fn format(values: &[String]) -> String {
    format!("[{}]", values.join(", "))
}
