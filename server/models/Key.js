const db = require('../config/database');

function generateKey() {
  const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
  let key = '';
  for (let i = 0; i < 16; i++) {
    if (i > 0 && i % 4 === 0) key += '-';
    key += chars.charAt(Math.floor(Math.random() * chars.length));
  }
  return key;
}

class Key {
  static create(type, days = null) {
    const key = generateKey();
    let expiresAt = null;
    if (type === 'trial' && days) {
      const date = new Date();
      date.setDate(date.getDate() + days);
      expiresAt = date.toISOString();
    }
    const stmt = db.prepare('INSERT INTO keys (key, type, expiresAt) VALUES (?, ?, ?)');
    const result = stmt.run(key, type, expiresAt);
    return { id: result.lastInsertRowid, key, type, expiresAt };
  }

  static findByKey(key) {
    const stmt = db.prepare('SELECT * FROM keys WHERE key = ?');
    return stmt.get(key);
  }

  static getAll() {
    const stmt = db.prepare(`
      SELECT k.*, u.username as usedByUsername
      FROM keys k
      LEFT JOIN users u ON k.usedBy = u.id
      ORDER BY k.createdAt DESC
    `);
    return stmt.all();
  }

  static getUnused() {
    const stmt = db.prepare('SELECT * FROM keys WHERE usedBy IS NULL ORDER BY createdAt DESC');
    return stmt.all();
  }

  static activate(key, userId) {
    const stmt = db.prepare('UPDATE keys SET usedBy = ? WHERE key = ? AND usedBy IS NULL');
    return stmt.run(userId, key);
  }

  static delete(id) {
    const stmt = db.prepare('DELETE FROM keys WHERE id = ?');
    return stmt.run(id);
  }

  static isValid(key) {
    const k = this.findByKey(key);
    if (!k || k.usedBy !== null) return false;
    if (k.type === 'trial' && k.expiresAt) {
      return new Date(k.expiresAt) > new Date();
    }
    return true;
  }
}

module.exports = Key;
