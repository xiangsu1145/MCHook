let token = localStorage.getItem('mchook_token');
let user = JSON.parse(localStorage.getItem('mchook_user') || 'null');

function show(elId) {
  document.getElementById(elId).classList.remove('hidden');
}

function hide(elId) {
  document.getElementById(elId).classList.add('hidden');
}

function checkAuth() {
  if (token && user && user.isAdmin) {
    show('admin-section');
    loadData();
  } else {
    show('login-section');
  }
}

async function adminLogin() {
  const username = document.getElementById('admin-username').value;
  const password = document.getElementById('admin-password').value;

  try {
    const res = await fetch('/api/auth/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password })
    });

    const data = await res.json();
    if (!res.ok) {
      document.getElementById('admin-error').textContent = data.error;
      return;
    }

    if (!data.user.isAdmin) {
      document.getElementById('admin-error').textContent = 'Admin access required';
      return;
    }

    token = data.token;
    user = data.user;
    localStorage.setItem('mchook_token', token);
    localStorage.setItem('mchook_user', JSON.stringify(user));
    checkAuth();
  } catch (err) {
    document.getElementById('admin-error').textContent = 'Connection error';
  }
}

function logout() {
  token = null;
  user = null;
  localStorage.removeItem('mchook_token');
  localStorage.removeItem('mchook_user');
  document.getElementById('admin-username').value = '';
  document.getElementById('admin-password').value = '';
  checkAuth();
}

async function loadData() {
  await Promise.all([loadKeys(), loadUsers()]);
}

async function loadKeys() {
  try {
    const res = await fetch('/api/admin/keys', {
      headers: { 'Authorization': `Bearer ${token}` }
    });
    const keys = await res.json();

    const tbody = document.getElementById('keys-table');
    tbody.innerHTML = keys.map(k => {
      let expiresText = '-';
      let statusClass = '';
      if (k.expiresAt) {
        const exp = new Date(k.expiresAt);
        expiresText = exp.toLocaleDateString();
        if (exp < new Date()) {
          expiresText += ' <span class="status-expired">(Expired)</span>';
        }
      } else {
        expiresText = '<span class="status-online">Never</span>';
      }

      return `
        <tr>
          <td><code>${k.key}</code></td>
          <td><span class="badge badge-${k.type}">${k.type}</span></td>
          <td>${expiresText}</td>
          <td>${k.usedByUsername || '<span class="badge badge-unused">Unused</span>'}</td>
          <td><button class="action-btn" onclick="deleteKey(${k.id})">Delete</button></td>
        </tr>
      `;
    }).join('');
  } catch (err) {
    console.error('Failed to load keys:', err);
  }
}

async function loadUsers() {
  try {
    const res = await fetch('/api/admin/users', {
      headers: { 'Authorization': `Bearer ${token}` }
    });
    const users = await res.json();

    const tbody = document.getElementById('users-table');
    tbody.innerHTML = users.map(u => `
      <tr>
        <td>${u.id}</td>
        <td>${u.username}</td>
        <td><span class="badge badge-${u.isAdmin ? 'admin' : 'user'}">${u.isAdmin ? 'Admin' : 'User'}</span></td>
        <td>${new Date(u.createdAt).toLocaleDateString()}</td>
        <td><button class="action-btn" onclick="deleteUser(${u.id})">Delete</button></td>
      </tr>
    `).join('');
  } catch (err) {
    console.error('Failed to load users:', err);
  }
}

async function generateKey() {
  const type = document.getElementById('key-type').value;
  const days = type === 'trial' ? parseInt(document.getElementById('key-days').value) : null;

  if (type === 'trial' && (!days || days < 1)) {
    alert('Please enter valid days for trial key');
    return;
  }

  try {
    const res = await fetch('/api/admin/keys', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${token}`
      },
      body: JSON.stringify({ type, days })
    });

    const key = await res.json();
    document.getElementById('generated-key').textContent = `Generated: ${key.key}`;
    loadKeys();
  } catch (err) {
    alert('Failed to generate key');
  }
}

async function deleteKey(id) {
  if (!confirm('Delete this key?')) return;

  try {
    await fetch(`/api/admin/keys/${id}`, {
      method: 'DELETE',
      headers: { 'Authorization': `Bearer ${token}` }
    });
    loadKeys();
  } catch (err) {
    alert('Failed to delete key');
  }
}

async function deleteUser(id) {
  if (!confirm('Delete this user?')) return;

  try {
    await fetch(`/api/admin/users/${id}`, {
      method: 'DELETE',
      headers: { 'Authorization': `Bearer ${token}` }
    });
    loadUsers();
  } catch (err) {
    alert('Failed to delete user');
  }
}

checkAuth();
