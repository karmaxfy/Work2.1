// Односвязный список: каждый узел имеет ссылку next.
struct Node {
    value: String,
    next: Option<Box<Node>>,
}

#[derive(Default)]
pub struct SinglyList {
    head: Option<Box<Node>>,
}

impl SinglyList {
    pub fn push_front(&mut self, value: String) {
        let node = Box::new(Node { value, next: self.head.take() });
        self.head = Some(node);
    }

    pub fn push_back(&mut self, value: String) {
        let mut link = &mut self.head;
        while link.is_some() {
            link = &mut link.as_mut().unwrap().next;
        }
        *link = Some(Box::new(Node { value, next: None }));
    }

    pub fn before(&mut self, target: &str, value: String) -> bool {
        let mut link = &mut self.head;
        loop {
            let found = match link.as_ref() {
                Some(node) => node.value == target,
                None => return false,
            };
            if found {
                let old = link.take();
                *link = Some(Box::new(Node { value, next: old }));
                return true;
            }
            link = &mut link.as_mut().unwrap().next;
        }
    }

    pub fn after(&mut self, target: &str, value: String) -> bool {
        let mut current = self.head.as_mut();
        while let Some(node) = current {
            if node.value == target {
                let next = node.next.take();
                node.next = Some(Box::new(Node { value, next }));
                return true;
            }
            current = node.next.as_mut();
        }
        false
    }

    pub fn pop_front(&mut self) -> Result<String, String> {
        let mut first = self.head.take().ok_or("список пуст")?;
        self.head = first.next.take();
        Ok(first.value)
    }

    fn pop_last(link: &mut Option<Box<Node>>) -> Result<String, String> {
        if link.is_none() {
            return Err("список пуст".into());
        }
        if link.as_ref().unwrap().next.is_none() {
            return Ok(link.take().unwrap().value);
        }
        Self::pop_last(&mut link.as_mut().unwrap().next)
    }

    pub fn pop_back(&mut self) -> Result<String, String> {
        Self::pop_last(&mut self.head)
    }

    pub fn remove(&mut self, target: &str) -> bool {
        let mut link = &mut self.head;
        loop {
            let found = match link.as_ref() {
                Some(node) => node.value == target,
                None => return false,
            };
            if found {
                let next = link.as_mut().unwrap().next.take();
                *link = next;
                return true;
            }
            link = &mut link.as_mut().unwrap().next;
        }
    }

    // Поиск позиции первого вхождения значения.
    fn position(&self, target: &str) -> Option<usize> {
        let mut current = self.head.as_ref();
        let mut index = 0;
        while let Some(node) = current {
            if node.value == target { return Some(index); }
            current = node.next.as_ref();
            index += 1;
        }
        None
    }

    // Удаление по позиции помогает реализовать удаление до/после значения.
    fn erase_at(&mut self, index: usize) -> Result<String, String> {
        let mut link = &mut self.head;
        for _ in 0..index {
            link = match link {
                Some(node) => &mut node.next,
                None => return Err("индекс вне диапазона".into()),
            };
        }
        let mut removed = link.take().ok_or("индекс вне диапазона")?;
        *link = removed.next.take();
        Ok(removed.value)
    }

    pub fn remove_before(&mut self, target: &str) -> bool {
        match self.position(target) {
            Some(index) if index > 0 => self.erase_at(index - 1).is_ok(),
            _ => false,
        }
    }

    pub fn remove_after(&mut self, target: &str) -> bool {
        match self.position(target) {
            Some(index) => self.erase_at(index + 1).is_ok(),
            None => false,
        }
    }

    pub fn front(&self) -> Result<String, String> {
        self.head.as_ref().map(|node| node.value.clone()).ok_or("список пуст".into())
    }

    pub fn back(&self) -> Result<String, String> {
        let mut current = self.head.as_ref().ok_or("список пуст")?;
        while let Some(next) = current.next.as_ref() { current = next; }
        Ok(current.value.clone())
    }

    pub fn get(&self, index: usize) -> Result<String, String> {
        let mut current = self.head.as_ref();
        for _ in 0..index {
            current = current.and_then(|node| node.next.as_ref());
        }
        current.map(|node| node.value.clone()).ok_or("индекс вне диапазона".into())
    }

    pub fn contains(&self, value: &str) -> bool {
        let mut current = self.head.as_ref();
        while let Some(node) = current {
            if node.value == value { return true; }
            current = node.next.as_ref();
        }
        false
    }

    pub fn values(&self) -> Vec<String> {
        let mut result = Vec::new();
        let mut current = self.head.as_ref();
        while let Some(node) = current {
            result.push(node.value.clone());
            current = node.next.as_ref();
        }
        result
    }
}
