use std::collections::LinkedList;

// Двусвязный список реализован стандартным LinkedList.
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

    // Удаление соседей у первого найденного значения.
    pub fn remove_before(&mut self, target: &str) -> bool {
        match self.items.iter().position(|value| value == target) {
            Some(index) if index > 0 => self.erase_at(index - 1),
            _ => false,
        }
    }

    pub fn remove_after(&mut self, target: &str) -> bool {
        match self.items.iter().position(|value| value == target) {
            Some(index) if index + 1 < self.items.len() => self.erase_at(index + 1),
            _ => false,
        }
    }

    fn erase_at(&mut self, index: usize) -> bool {
        if index >= self.items.len() { return false; }
        let mut tail = self.items.split_off(index);
        let removed = tail.pop_front().is_some();
        self.items.append(&mut tail);
        removed
    }

    pub fn front(&self) -> Result<String, String> {
        self.items.front().cloned().ok_or("список пуст".into())
    }

    pub fn back(&self) -> Result<String, String> {
        self.items.back().cloned().ok_or("список пуст".into())
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
