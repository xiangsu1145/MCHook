const express = require('express');
const router = express.Router();
const User = require('../models/User');
const Key = require('../models/Key');
const { authenticate, generateToken } = require('../middleware/auth');

router.post('/register', (req, res) => {
  const { username, password } = req.body;
  if (!username || !password) {
    return res.status(400).json({ error: 'Username and password required' });
  }
  if (username.length < 3 || password.length < 6) {
    return res.status(400).json({ error: 'Username must be 3+ chars, password 6+ chars' });
  }
  try {
    const existing = User.findByUsername(username);
    if (existing) {
      return res.status(400).json({ error: 'Username already exists' });
    }
    const userId = User.create(username, password);
    const user = User.findById(userId);
    const token = generateToken(user);
    res.json({ token, user: { id: user.id, username: user.username, isAdmin: user.isAdmin } });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.post('/login', (req, res) => {
  const { username, password } = req.body;
  if (!username || !password) {
    return res.status(400).json({ error: 'Username and password required' });
  }
  try {
    const user = User.findByUsername(username);
    if (!user || !User.verifyPassword(password, user.password)) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }
    const token = generateToken(user);
    res.json({ token, user: { id: user.id, username: user.username, isAdmin: user.isAdmin } });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.post('/activate', authenticate, (req, res) => {
  const { key } = req.body;
  if (!key) {
    return res.status(400).json({ error: 'Key required' });
  }
  try {
    if (!Key.isValid(key)) {
      return res.status(400).json({ error: 'Invalid or expired key' });
    }
    Key.activate(key, req.user.id);
    res.json({ message: 'Account activated successfully' });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.get('/me', authenticate, (req, res) => {
  res.json({ id: req.user.id, username: req.user.username, isAdmin: req.user.isAdmin });
});

module.exports = router;
