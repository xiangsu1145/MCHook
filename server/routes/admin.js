const express = require('express');
const router = express.Router();
const User = require('../models/User');
const Key = require('../models/Key');
const { authenticate, requireAdmin } = require('../middleware/auth');

router.use(authenticate);
router.use(requireAdmin);

router.get('/users', (req, res) => {
  try {
    const users = User.getAll();
    res.json(users);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.delete('/users/:id', (req, res) => {
  try {
    if (req.user.id === parseInt(req.params.id)) {
      return res.status(400).json({ error: 'Cannot delete yourself' });
    }
    User.delete(req.params.id);
    res.json({ message: 'User deleted' });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.get('/keys', (req, res) => {
  try {
    const keys = Key.getAll();
    res.json(keys);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.post('/keys', (req, res) => {
  const { type, days } = req.body;
  if (!type || !['trial', 'permanent'].includes(type)) {
    return res.status(400).json({ error: 'Invalid key type' });
  }
  if (type === 'trial' && (!days || days < 1)) {
    return res.status(400).json({ error: 'Trial keys require valid days' });
  }
  try {
    const key = Key.create(type, days);
    res.json(key);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.delete('/keys/:id', (req, res) => {
  try {
    Key.delete(req.params.id);
    res.json({ message: 'Key deleted' });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

module.exports = router;
