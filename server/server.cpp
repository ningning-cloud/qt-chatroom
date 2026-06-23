#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <string>
#include<ctime>

//获取当前时间字符串,格式 HH:MM:SS
std::string now_time() {
    time_t t = time(nullptr);
    struct tm* lt = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M:%S", lt);
    return std::string(buf);
}

std::vector<int> clients;
std::mutex clients_mutex;

//存储客户端 fd -> 用户名 的映射
std::map<int, std::string> users;
std::mutex users_mutex;

void handleClient(int client_fd) {
    char buffer[1024] = {0};

    // 第一步：等待客户端发送 LOGIN 消息
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0 && strncmp(buffer, "LOGIN:", 6) == 0) {
        std::string username = std::string(buffer + 6);
        std::lock_guard<std::mutex> lock(users_mutex);
        users[client_fd] = username;
        std::cout << "[" << now_time() << "] [登录] " << username << " 已上线" << std::endl;
{
    std::lock_guard<std::mutex> lock2(clients_mutex);
    std::cout << "当前 " << clients.size() << " 人在线" << std::endl;
}
    }

    else {
        close(client_fd);
        return;
    }

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

  // 获取该 fd 对应的用户名
        std::string username;
        {
            std::lock_guard<std::mutex> lock(users_mutex);
            username = users[client_fd];
        }

        // 断开连接的处理（仅当 bytes_received <= 0 时才进这里）
        if (bytes_received <= 0) {
std::cout << "[" << now_time() << "] [断开] " << username << " 已下线 (fd=" << client_fd << ")" << std::endl;

            // 从 clients 移除
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    if (*it == client_fd) {
                        clients.erase(it);
                        break;
                    }
                }
            }  //clients_mutex 在这里释放

            // 从 users 移除
            {
                std::lock_guard<std::mutex> lock(users_mutex);
                users.erase(client_fd);
            }  //users_mutex 在这里释放

            close(client_fd);
            break;  // 跳出 while，函数结束
        }

// 去掉末尾的换行符
std::string content(buffer);
while (!content.empty() && (content.back() == '\n' || content.back() == '\r')) {
    content.pop_back();
}

// 过滤空消息
if (content.empty()) {
    continue;  // 跳过本次循环，不广播
}

        // 构造带用户名的消息
        std::string msg = username + ": " + content+ "\n";
        std::cout << "[" << now_time() << "] [消息] " << msg << std::endl;
        // 广播给所有在线客户端
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int fd : clients) {
            if (fd != client_fd) {
                send(fd, msg.c_str(), msg.size(), 0);
            }
        }
    }
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        std::cerr << "socket 创建失败" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    //设置 SO_REUSEADDR，避免重启时 bind 失败
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "bind 绑定失败" << std::endl;
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, 5) == -1) {
        std::cerr << "listen 监听失败" << std::endl;
        close(listen_fd);
        return -1;
    }

    std::cout << "===============================" << std::endl;
    std::cout << " 服务器已启动 | 端口: 8888" << std::endl;
    std::cout << "===============================" << std::endl;

    while(true){
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            std::cerr << "accept 失败" << std::endl;
            continue;  //accept 失败不应该退出服务器，应为 continue
        }

        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_fd);
       std::cout << "[" << now_time() << "] [连接] 新客户端: " << inet_ntoa(client_addr.sin_addr) << std::endl;

        std::thread t(handleClient, client_fd);
        t.detach();
    }

    close(listen_fd);
    return 0;
}
