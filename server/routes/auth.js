const express = require('express');
const router = express.Router();
const User = require('../models/User');
const Key = require('../models/Key');
const { authenticate, generateToken } = require('../middleware/auth');

router.post('/register', (req, res) => {
  const { username, password } = req.body;
  if (!username || !password) {
    return res.status(400).json({ error: '用户名和密码不能为空' });
  }
  if (username.length < 3 || password.length < 6) {
    return res.status(400).json({ error: '用户名至少3个字符，密码至少6个字符' });
  }
  try {
    const existing = User.findByUsername(username);
    if (existing) {
      return res.status(400).json({ error: '用户名已存在' });
    }
    const userId = User.create(username, password);
    const user = User.findById(userId);
    const token = generateToken(user);
    res.json({ token, user: { id: user.id, username: user.username, isAdmin: user.isAdmin, activated: user.activated } });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.post('/login', (req, res) => {
  const { username, password } = req.body;
  if (!username || !password) {
    return res.status(400).json({ error: '用户名和密码不能为空' });
  }
  try {
    const user = User.findByUsername(username);
    if (!user || !User.verifyPassword(password, user.password)) {
      return res.status(401).json({ error: '用户名或密码错误' });
    }
    const token = generateToken(user);
    res.json({ token, user: { id: user.id, username: user.username, isAdmin: user.isAdmin, activated: user.activated } });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.post('/activate', authenticate, (req, res) => {
  const { key } = req.body;
  if (!key) {
    return res.status(400).json({ error: '密钥不能为空' });
  }
  try {
    if (!Key.isValid(key)) {
      return res.status(400).json({ error: '无效或已使用的密钥' });
    }
    Key.activate(key, req.user.id);
    User.activate(req.user.id);
    res.json({ message: '激活成功' });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.get('/me', authenticate, (req, res) => {
  const user = User.findById(req.user.id);
  if (!user) {
    return res.status(404).json({ error: '用户不存在' });
  }

  const response = {
    id: user.id,
    username: user.username,
    isAdmin: user.isAdmin,
    activated: !!user.activated,
    createdAt: user.createdAt,
    keyType: null,
    expiresAt: null
  };

  if (user.activated) {
    const keyResult = require('../config/database').getDatabase().exec(
      'SELECT type, expiresAt FROM keys WHERE usedBy = ? ORDER BY createdAt DESC LIMIT 1',
      [user.id]
    );
    if (keyResult.length > 0 && keyResult[0].values.length > 0) {
      response.keyType = keyResult[0].values[0][0];
      response.expiresAt = keyResult[0].values[0][1];
    }
  }

  res.json(response);
});

module.exports = router;
