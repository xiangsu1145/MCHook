let token = localStorage.getItem('mchook_token');

document.querySelectorAll('.tab').forEach(tab => {
  tab.addEventListener('click', () => {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    tab.classList.add('active');

    const tabName = tab.dataset.tab;
    if (tabName === 'login') {
      document.getElementById('login-form').classList.remove('hidden');
      document.getElementById('register-form').classList.add('hidden');
    } else {
      document.getElementById('login-form').classList.add('hidden');
      document.getElementById('register-form').classList.remove('hidden');
    }
  });
});

function showError(elId, msg) {
  const el = document.getElementById(elId);
  el.textContent = msg;
  el.classList.remove('hidden');
  setTimeout(() => el.classList.add('hidden'), 3000);
}

function showSuccess(elId, msg) {
  const el = document.getElementById(elId);
  el.textContent = msg;
  el.classList.remove('hidden');
  setTimeout(() => el.classList.add('hidden'), 3000);
}

async function login() {
  const username = document.getElementById('login-username').value;
  const password = document.getElementById('login-password').value;

  if (!username || !password) {
    showError('login-error', 'Please fill in all fields');
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
    window.location.href = '/admin';
  } catch (err) {
    showError('login-error', 'Connection error');
  }
}

async function register() {
  const username = document.getElementById('reg-username').value;
  const password = document.getElementById('reg-password').value;
  const key = document.getElementById('reg-key').value;

  if (!username || !password || !key) {
    showError('reg-error', 'Please fill in all fields');
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

    const activateRes = await fetch('/api/auth/activate', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${data.token}`
      },
      body: JSON.stringify({ key })
    });

    const activateData = await activateRes.json();
    if (!activateRes.ok) {
      showError('reg-error', 'Registration ok but activation failed: ' + activateData.error);
      return;
    }

    showSuccess('reg-success', 'Registration and activation successful!');
    setTimeout(() => {
      document.querySelector('.tab[data-tab="login"]').click();
    }, 1500);
  } catch (err) {
    showError('reg-error', 'Connection error');
  }
}

if (token) {
  window.location.href = '/admin';
}
