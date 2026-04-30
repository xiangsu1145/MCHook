const db = require('../config/database');
const bcrypt = require('bcryptjs');

class User {
  static create(username, password, isAdmin = false) {
    const hash = bcrypt.hashSync(password, 10);
    const stmt = db.prepare('INSERT INTO users (username, password, isAdmin) VALUES (?, ?, ?)');
    const result = stmt.run(username, hash, isAdmin ? 1 : 0);
    return result.lastInsertRowid;
  }

  static findByUsername(username) {
    const stmt = db.prepare('SELECT * FROM users WHERE username = ?');
    return stmt.get(username);
  }

  static findById(id) {
    const stmt = db.prepare('SELECT * FROM users WHERE id = ?');
    return stmt.get(id);
  }

  static getAll() {
    const stmt = db.prepare('SELECT id, username, isAdmin, createdAt FROM users ORDER BY createdAt DESC');
    return stmt.all();
  }

  static delete(id) {
    const stmt = db.prepare('DELETE FROM users WHERE id = ?');
    return stmt.run(id);
  }

  static verifyPassword(password, hash) {
    return bcrypt.compareSync(password, hash);
  }
}

module.exports = User;
