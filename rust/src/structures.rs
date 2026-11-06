use std::collections::LinkedList;

// Динамический массив: Vec сам управляет выделением памяти.
#[derive(Default)]
pub struct Array {
    items: Vec<String>,
}

impl Array {
    pub fn push(&mut self, value: String) {
        self.items.push(value);
    }

    pub fn insert(&mut self, index: usize, value: String) -> Result<(), String> {
        if index > self.items.len() {
            return Err("индекс вне диапазона".into());
        }
        self.items.insert(index, value);
        Ok(())
    }

    pub fn get(&self, index: usize) -> Result<String, String> {
        self.items.get(index).cloned().ok_or("индекс вне диапазона".into())
    }

    pub fn set(&mut self, index: usize, value: String) -> Result<(), String> {
        let cell = self.items.get_mut(index).ok_or("индекс вне диапазона")?;
        *cell = value;
        Ok(())
    }

    pub fn erase(&mut self, index: usize) -> Result<String, String> {
        if index >= self.items.len() {
            return Err("индекс вне диапазона".into());
        }
        Ok(self.items.remove(index))
    }

    pub fn len(&self) -> usize {
        self.items.len()
    }

    pub fn values(&self) -> Vec<String> {
        self.items.clone()
    }
}

// Односвязный список: у каждого узла только ссылка next.
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

    pub fn get(&self, index: usize) -> Result<String, String> {
        let mut current = self.head.as_ref();
        for _ in 0..index {
            current = current.and_then(|node| node.next.as_ref());
        }
        current.map(|node| node.value.clone()).ok_or("индекс вне диапазона".into())
    }

    pub fn contains(&self, value: &str) -> bool {
        self.values().iter().any(|item| item == value)
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

// Двусвязный список реализован безопасным LinkedList из стандартной библиотеки.
#[derive(Default)]
pub struct DoublyList {
    items: LinkedList<String>,
}

impl DoublyList {
    pub fn push_front(&mut self, value: String) {
        self.items.push_front(value);
    }

    pub fn push_back(&mut self, value: String) {
        self.items.push_back(value);
    }

    pub fn before(&mut self, target: &str, value: String) -> bool {
        let index = match self.items.iter().position(|x| x == target) {
            Some(index) => index,
            None => return false,
        };
        let mut tail = self.items.split_off(index);
        self.items.push_back(value);
        self.items.append(&mut tail);
        true
    }

    pub fn after(&mut self, target: &str, value: String) -> bool {
        let index = match self.items.iter().position(|x| x == target) {
            Some(index) => index + 1,
            None => return false,
        };
        let mut tail = self.items.split_off(index);
        self.items.push_back(value);
        self.items.append(&mut tail);
        true
    }

    pub fn remove(&mut self, target: &str) -> bool {
        let index = match self.items.iter().position(|x| x == target) {
            Some(index) => index,
            None => return false,
        };
        let mut tail = self.items.split_off(index);
        tail.pop_front();
        self.items.append(&mut tail);
        true
    }

    pub fn pop_front(&mut self) -> Result<String, String> {
        self.items.pop_front().ok_or("список пуст".into())
    }

    pub fn pop_back(&mut self) -> Result<String, String> {
        self.items.pop_back().ok_or("список пуст".into())
    }

    pub fn get(&self, index: usize) -> Result<String, String> {
        self.items.iter().nth(index).cloned().ok_or("индекс вне диапазона".into())
    }

    pub fn contains(&self, value: &str) -> bool {
        self.items.iter().any(|item| item == value)
    }

    pub fn values(&self) -> Vec<String> {
        self.items.iter().cloned().collect()
    }

    pub fn reverse(&self) -> Vec<String> {
        self.items.iter().rev().cloned().collect()
    }
}

// Стек LIFO использует голову односвязного списка.
#[derive(Default)]
pub struct Stack {
    list: SinglyList,
}

impl Stack {
    pub fn push(&mut self, value: String) {
        self.list.push_front(value);
    }

    pub fn pop(&mut self) -> Result<String, String> {
        self.list.pop_front()
    }

    pub fn get(&self) -> Result<String, String> {
        self.list.get(0)
    }

    pub fn values(&self) -> Vec<String> {
        self.list.values()
    }
}

// Очередь FIFO: добавляем в хвост, забираем из головы.
#[derive(Default)]
pub struct Queue {
    list: DoublyList,
}

impl Queue {
    pub fn push(&mut self, value: String) {
        self.list.push_back(value);
    }

    pub fn pop(&mut self) -> Result<String, String> {
        self.list.pop_front()
    }

    pub fn get(&self) -> Result<String, String> {
        self.list.get(0)
    }

    pub fn values(&self) -> Vec<String> {
        self.list.values()
    }
}
