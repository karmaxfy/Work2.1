use super::DoublyList;

// Очередь FIFO использует двусвязный список.
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
