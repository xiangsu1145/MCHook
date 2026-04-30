# MCHook Server

## 安装

```bash
cd server
npm install
```

## 启动

```bash
npm start
```

服务器运行在 http://localhost:3000

## 初始化Admin

1. 访问 http://localhost:3000/admin
2. 注册一个新账号
3. 用数据库工具（如 sqlite3）手动将 isAdmin 设为 1
4. 登录 admin 面板

或者直接插入 admin 用户到数据库：

```sql
INSERT INTO users (username, password, isAdmin) VALUES ('admin', '$2a$10$...', 1);
```

（密码需要 bcrypt 加密）

## API 路由

### 公开
- POST /api/auth/register - 注册
- POST /api/auth/login - 登录

### 需要认证
- POST /api/auth/activate - 使用密钥激活
- GET /api/auth/me - 获取当前用户

### Admin (需要 isAdmin=1)
- GET /api/admin/users - 获取所有用户
- DELETE /api/admin/users/:id - 删除用户
- GET /api/admin/keys - 获取所有密钥
- POST /api/admin/keys - 生成密钥
- DELETE /api/admin/keys/:id - 删除密钥

## 目录结构

```
server/
├── server.js          # 入口
├── config/           # 配置
├── models/           # 数据模型
├── routes/           # 路由
├── middleware/       # 中间件
└── public/           # 前端静态文件
```
