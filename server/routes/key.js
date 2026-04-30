const express = require('express');
const router = express.Router();
const Key = require('../models/Key');
const { authenticate } = require('../middleware/auth');

router.get('/generate', authenticate, (req, res) => {
  try {
    const key = Key.create('trial', 1);
    res.json(key);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

router.get('/unused', (req, res) => {
  try {
    const keys = Key.getUnused();
    res.json(keys);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

module.exports = router;
