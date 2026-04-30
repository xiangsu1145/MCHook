let token = localStorage.getItem('mchook_token');
let currentUser = JSON.parse(localStorage.getItem('mchook_user') || 'null');

document.querySelectorAll('.tab').forEach(tab => {
  tab.addEventListener('click', () => {
    if (currentUser) return;
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    tab.classList.add('active');
    const tabName = tab.dataset.tab;
    document.getElementById('login-form').classList.add('hidden');
    document.getElementById('register-form').classList.add('hidden');
    if (tabName === 'login') {
      document.getElementById('login-form').classList.remove('hidden');
    } else if (tabName === 'register') {
      document.getElementById('register-form').classList.remove('hidden');
    }
  });
});

function showError(elId, msg) {
  const el = document.getElementById(elId);
  el.textContent = msg;
  el.classList.remove('hidden');
  setTimeout(() => el.classList.add('hidden'), 5000);
}

function showSuccess(elId, msg) {
  const el = document.getElementById(elId);
  el.textContent = msg;
  el.classList.remove('hidden');
  setTimeout(() => el.classList.add('hidden'), 5000);
}

async function login() {
  const username = document.getElementById('login-username').value;
  const password = document.getElementById('login-password').value;
  if (!username || !password) {
    showError('login-error', '请填写所有字段');
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
      showError('login-error', data.error);
      return;
    }
    localStorage.setItem('mchook_token', data.token);
    localStorage.setItem('mchook_user', JSON.stringify(data.user));
    if (data.user.isAdmin) {
      window.location.href = '/admin';
    } else {
      window.location.href = '/user';
    }
  } catch (err) {
    showError('login-error', '连接错误');
  }
}

async function register() {
  const username = document.getElementById('reg-username').value;
  const password = document.getElementById('reg-password').value;
  if (!username || !password) {
    showError('reg-error', '请填写所有字段');
    return;
  }
  if (username.length < 3 || password.length < 6) {
    showError('reg-error', '用户名至少3个字符，密码至少6个字符');
    return;
  }
  try {
    const res = await fetch('/api/auth/register', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password })
    });
    const data = await res.json();
    if (!res.ok) {
      showError('reg-error', data.error);
      return;
    }
    localStorage.setItem('mchook_token', data.token);
    localStorage.setItem('mchook_user', JSON.stringify(data.user));
    showSuccess('reg-success', '注册成功！正在跳转...');
    setTimeout(() => {
      window.location.href = '/user';
    }, 1000);
  } catch (err) {
    showError('reg-error', '连接错误');
  }
}

if (token && currentUser) {
  if (currentUser.isAdmin) {
    window.location.href = '/admin';
  } else {
    window.location.href = '/user';
  }
}
