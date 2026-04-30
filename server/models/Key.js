const { getDatabase, saveDatabase } = require('../config/database');

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
    const db = getDatabase();
    const key = generateKey();
    let expiresAt = null;
    if (type === 'trial' && days) {
      const date = new Date();
      date.setDate(date.getDate() + days);
      expiresAt = date.toISOString();
    }
    db.run('INSERT INTO keys (key, type, expiresAt) VALUES (?, ?, ?)', [key, type, expiresAt]);
    saveDatabase();
    const result = db.exec('SELECT last_insert_rowid() as id');
    return { id: result[0].values[0][0], key, type, expiresAt };
  }

  static findByKey(key) {
    const db = getDatabase();
    const result = db.exec('SELECT * FROM keys WHERE key = ?', [key]);
    if (result.length === 0 || result[0].values.length === 0) return null;
    const columns = result[0].columns;
    const values = result[0].values[0];
    const k = {};
    columns.forEach((col, i) => k[col] = values[i]);
    return k;
  }

  static getAll() {
    const db = getDatabase();
    const result = db.exec(`
      SELECT k.id, k.key, k.type, k.expiresAt, k.usedBy, k.createdAt, u.username as usedByUsername
      FROM keys k
      LEFT JOIN users u ON k.usedBy = u.id
      ORDER BY k.createdAt DESC
    `);
    if (result.length === 0) return [];
    const columns = result[0].columns;
    return result[0].values.map(values => {
      const k = {};
      columns.forEach((col, i) => k[col] = values[i]);
      return k;
    });
  }

  static getUnused() {
    const db = getDatabase();
    const result = db.exec('SELECT * FROM keys WHERE usedBy IS NULL ORDER BY createdAt DESC');
    if (result.length === 0) return [];
    const columns = result[0].columns;
    return result[0].values.map(values => {
      const k = {};
      columns.forEach((col, i) => k[col] = values[i]);
      return k;
    });
  }

  static activate(key, userId) {
    const db = getDatabase();
    db.run('UPDATE keys SET usedBy = ? WHERE key = ? AND usedBy IS NULL', [userId, key]);
    saveDatabase();
  }

  static delete(id) {
    const db = getDatabase();
    db.run('DELETE FROM keys WHERE id = ?', [id]);
    saveDatabase();
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
