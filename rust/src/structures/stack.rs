use super::SinglyList;

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
