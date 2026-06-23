# Qt TCP 聊天室

基于 Qt 和 Linux Socket 的 TCP 网络聊天室，支持多客户端群聊。

## 功能

- 多客户端同时在线
- 用户登录
- 群聊广播（消息实时同步给所有在线用户）
- 跨平台：Windows Qt 客户端 ↔ Linux 服务端

## 技术栈

| 部分 | 技术 |
|------|------|
| 服务端 | C++11、Linux Socket、pthread、TCP/IP |
| 客户端 | Qt5/6、QTcpSocket、QThread、CMake |
| 工具 | WSL2、gcc/gdb、Git |

## 项目结构
