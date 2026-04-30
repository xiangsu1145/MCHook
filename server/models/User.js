const bcrypt = require('bcryptjs');
const { getDatabase, saveDatabase } = require('../config/database');

class User {
  static create(username, password, isAdmin = false) {
    const db = getDatabase();
    const hash = bcrypt.hashSync(password, 10);
    db.run('INSERT INTO users (username, password, isAdmin) VALUES (?, ?, ?)', [username, hash, isAdmin ? 1 : 0]);
    saveDatabase();
    const result = db.exec('SELECT last_insert_rowid() as id');
    return result[0].values[0][0];
  }

  static findByUsername(username) {
    const db = getDatabase();
    const result = db.exec('SELECT * FROM users WHERE username = ?', [username]);
    if (result.length === 0 || result[0].values.length === 0) return null;
    const columns = result[0].columns;
    const values = result[0].values[0];
    const user = {};
    columns.forEach((col, i) => user[col] = values[i]);
    return user;
  }

  static findById(id) {
    const db = getDatabase();
    const result = db.exec('SELECT * FROM users WHERE id = ?', [id]);
    if (result.length === 0 || result[0].values.length === 0) return null;
    const columns = result[0].columns;
    const values = result[0].values[0];
    const user = {};
    columns.forEach((col, i) => user[col] = values[i]);
    return user;
  }

  static getAll() {
    const db = getDatabase();
    const result = db.exec('SELECT id, username, isAdmin, activated, createdAt FROM users ORDER BY createdAt DESC');
    if (result.length === 0) return [];
    const columns = result[0].columns;
    return result[0].values.map(values => {
      const user = {};
      columns.forEach((col, i) => user[col] = values[i]);
      return user;
    });
  }

  static delete(id) {
    const db = getDatabase();
    db.run('DELETE FROM users WHERE id = ?', [id]);
    saveDatabase();
  }

  static activate(id) {
    const db = getDatabase();
    db.run('UPDATE users SET activated = 1 WHERE id = ?', [id]);
    saveDatabase();
  }

  static verifyPassword(password, hash) {
    return bcrypt.compareSync(password, hash);
  }
}

module.exports = User;
