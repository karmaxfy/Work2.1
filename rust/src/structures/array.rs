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
