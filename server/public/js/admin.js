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

  if (!username || !password) {
    document.getElementById('admin-error').textContent = '请填写所有字段';
    return;
  }

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
      window.location.href = '/user';
      return;
    }

    token = data.token;
    user = data.user;
    localStorage.setItem('mchook_token', token);
    localStorage.setItem('mchook_user', JSON.stringify(user));
    checkAuth();
  } catch (err) {
    document.getElementById('admin-error').textContent = '连接错误';
  }
}

function logout() {
  token = null;
  user = null;
  localStorage.removeItem('mchook_token');
  localStorage.removeItem('mchook_user');
  window.location.href = '/';
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
    if (keys.length === 0) {
      tbody.innerHTML = '<tr><td colspan="5" style="text-align:center;color:#999;">暂无密钥</td></tr>';
      return;
    }
    tbody.innerHTML = keys.map(k => {
      let expiresText = '-';
      if (k.expiresAt) {
        const exp = new Date(k.expiresAt);
        expiresText = exp.toLocaleDateString();
        if (exp < new Date()) {
          expiresText += ' <span class="status-expired">(已过期)</span>';
        }
      } else {
        expiresText = '<span class="status-online">永久</span>';
      }

      return `
        <tr>
          <td><code>${k.key}</code></td>
          <td><span class="badge badge-${k.type}">${k.type === 'trial' ? '试用' : '永久'}</span></td>
          <td>${expiresText}</td>
          <td>${k.usedByUsername || '<span class="badge badge-unused">未使用</span>'}</td>
          <td><button class="action-btn" onclick="deleteKey(${k.id})">删除</button></td>
        </tr>
      `;
    }).join('');
  } catch (err) {
    console.error('加载密钥失败:', err);
  }
}

async function loadUsers() {
  try {
    const res = await fetch('/api/admin/users', {
      headers: { 'Authorization': `Bearer ${token}` }
    });
    const users = await res.json();

    const tbody = document.getElementById('users-table');
    if (users.length === 0) {
      tbody.innerHTML = '<tr><td colspan="5" style="text-align:center;color:#999;">暂无用户</td></tr>';
      return;
    }
    tbody.innerHTML = users.map(u => `
      <tr>
        <td>${u.id}</td>
        <td>${u.username}</td>
        <td><span class="badge badge-${u.isAdmin ? 'admin' : 'user'}">${u.isAdmin ? '管理员' : '用户'}</span></td>
        <td><span style="color:${u.activated ? '#27ae60' : '#e74c3c'};">${u.activated ? '已激活' : '未激活'}</span></td>
        <td>${new Date(u.createdAt).toLocaleDateString()}</td>
        <td><button class="action-btn" onclick="deleteUser(${u.id})">删除</button></td>
      </tr>
    `).join('');
  } catch (err) {
    console.error('加载用户失败:', err);
  }
}

async function generateKey() {
  const type = document.getElementById('key-type').value;
  const days = type === 'trial' ? parseInt(document.getElementById('key-days').value) : null;

  if (type === 'trial' && (!days || days < 1)) {
    alert('请输入有效天数');
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
    document.getElementById('generated-key').textContent = `已生成：${key.key}`;
    loadKeys();
  } catch (err) {
    alert('生成密钥失败');
  }
}

async function deleteKey(id) {
  if (!confirm('确定删除此密钥？')) return;

  try {
    await fetch(`/api/admin/keys/${id}`, {
      method: 'DELETE',
      headers: { 'Authorization': `Bearer ${token}` }
    });
    loadKeys();
  } catch (err) {
    alert('删除失败');
  }
}

async function deleteUser(id) {
  if (!confirm('确定删除此用户？')) return;

  try {
    await fetch(`/api/admin/users/${id}`, {
      method: 'DELETE',
      headers: { 'Authorization': `Bearer ${token}` }
    });
    loadUsers();
  } catch (err) {
    alert('删除失败');
  }
}

checkAuth();
