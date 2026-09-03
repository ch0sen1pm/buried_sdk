# BuriedPoint —— 埋点上报 SDK

一个 **Windows C++20** 埋点（事件采集）SDK：把业务侧埋下的事件，**异步、可靠、加密、批量**地送到远端服务器。对外只暴露极简的 **C ABI**，编译成 DLL，任何语言都能调用。

## ✨ 特性

- **异步无锁**：`boost::asio` + `strand` 单线程事件循环，业务线程调用 `Report` 零阻塞，内部无需加锁
- **可靠投递**：事件先落本地 SQLite（Outbox 模式），服务器确认成功才删除，断网/崩溃不丢数据
- **加密存储**：AES-256-CBC（mbedtls）+ PBKDF2 派生密钥，敏感信息（device_id 等）不落明文
- **批量上报**：5s 定时器 + 按优先级取 TopN，一次 HTTP 发送多条，降低连接开销
- **跨语言接口**：C ABI（`extern "C"`）+ 不透明指针，DLL 导出，任何语言可 FFI 调用
- **工程化**：Pimpl 隐藏实现、Meyers 单例、CMake + vcpkg 依赖管理

## 🏗 架构

```
┌──────────────────────────────────────────┐
│  C API 层      buried.cc                 │  对外 DLL 接口，参数校验 + 桥接
├──────────────────────────────────────────┤
│  核心层        Buried (buried_core)      │  组装 logger + 上报引擎 + 工作目录
├──────────────────────────────────────────┤
│  上报引擎      BuriedReport              │  加密→落库→定时批量→HTTP
├────────┬──────────┬──────────┬───────────┤
│ Crypt  │ Database │ HttpReport│ CommonService│  四个零件，各司其职
├────────┴──────────┴──────────┴───────────┤
│  并发底座      Context                   │  双 io_context + 双 strand + 双线程
└──────────────────────────────────────────┘
```

## 🚀 构建

依赖 vcpkg，所有第三方库见根目录 `vcpkg.json`：

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DBUILD_BURIED_EXAMPLES=ON
cmake --build build --config Release
```

## 📦 使用示例

```c
#include "include/buried.h"

int main() {
  Buried* buried = Buried_Create("D:/buried");
  if (!buried) return -1;

  BuriedConfig config;
  config.host = "localhost";
  config.port = "5678";
  config.topic = "test_topic";
  config.user_id = "test_user";
  config.app_version = "1.0.0";
  config.app_name = "test_app";
  config.custom_data = "{\"test\":\"test\"}";
  Buried_Start(buried, &config);

  Buried_Report(buried, "button_click", "{\"page\":\"home\"}", 5);

  Buried_Destroy(buried);
  return 0;
}
```

## 🧩 设计要点

| 设计 | 解决的问题 |
|---|---|
| 先落库再上报（Outbox） | 持久化兜底，断网/崩溃不丢数据 |
| strand 串行化，无锁 | 多线程安全，免去锁的复杂度与死锁风险 |
| 加密落盘 | 敏感信息不落明文 |
| 5s 定时 + TopN 攒批 | 高频小请求 → 低频批量请求 |
| C ABI + 不透明指针 | 跨语言复用 |
| Pimpl | 隐藏实现、ABI 稳定、编译隔离 |

## 📂 目录结构

```
├── include/          对外公开头文件（C ABI）
├── src/
│   ├── buried.cc     C API 实现
│   ├── buried_core   Buried 核心类
│   ├── context/      并发底座（双 io_context + strand + 线程）
│   ├── common/       设备/系统信息采集
│   ├── crypt/        AES-256-CBC 加密
│   ├── database/     SQLite 本地缓存
│   └── report/       上报引擎 + HTTP 客户端
├── examples/         调用示例
├── server/           本地测试接收服务器
├── tests/            单元测试
└── vcpkg.json        依赖清单
```

## 🗺 Roadmap

### 基础 & 构建

- [ ] `vcpkg.json` 依赖清单（boost-asio/beast/date-time、spdlog、mbedtls、sqlite-orm、nlohmann-json）
- [ ] 顶层 `CMakeLists.txt` + `src/CMakeLists.txt`（find_package + target_link_libraries）
- [x] README + .gitignore

### 核心模块（自底向上，按依赖顺序）

- [x] **Context** —— 并发底座：双 io_context + 双 strand + 双线程，Meyers 单例
- [ ] **CommonService** —— 设备信息采集：device_id（注册表持久化）、系统版本、机器名、进程时间、随机 id
- [ ] **Crypt** —— AES-256-CBC 加解密，PBKDF2 派生密钥（mbedtls）
- [ ] **Database** —— SQLite 本地缓存，sqlite_orm 建表 / 增删查（按优先级 + limit）
- [ ] **HttpReporter** —— boost.beast 同步 HTTP POST 客户端
- [ ] **BuriedReport** —— 上报引擎：InsertData → 加密落库 → 5s 定时批量 → 解密组 JSON → HTTP → 成功删除
- [ ] **Buried 核心 + C API** —— buried_core + buried.cc + include/buried.h，打通 DLL 接口

### 验证 & 收尾

- [ ] **examples** —— 调用示例，编译跑通
- [ ] **server** —— 本地接收服务器，端到端联调
- [ ] **tests** —— 单元测试（gtest）
