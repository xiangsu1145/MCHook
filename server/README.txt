MCHook Server
=============

安装与启动
-----------

1. 运行 init.bat 安装依赖并重置数据库
2. 运行 start.bat 启动服务器
3. 访问 http://localhost:3000

默认管理员账号: admin / admin123

功能说明
--------

用户端 (/):
  - 注册账号
  - 登录
  - 用户中心 (/user) - 查看账号状态、激活账号

管理端 (/admin):
  - 登录 (admin/admin123)
  - 生成密钥 (永久/试用)
  - 管理密钥
  - 管理用户

API 路由
--------

公开:
  POST /api/auth/register  - 注册
  POST /api/auth/login    - 登录

用户 (需认证):
  POST /api/auth/activate - 激活账号
  GET  /api/auth/me      - 获取用户信息

管理员 (需admin权限):
  GET  /api/admin/users   - 获取所有用户
  DELETE /api/admin/users/:id - 删除用户
  GET  /api/admin/keys    - 获取所有密钥
  POST /api/admin/keys    - 生成密钥
  DELETE /api/admin/keys/:id - 删除密钥

数据结构
--------

用户表 users:
  id, username, password, isAdmin, activated, createdAt

密钥表 keys:
  id, key, type, expiresAt, usedBy, createdAt
