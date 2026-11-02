# 构建模块交接

适用项目：`E:\Pocket Heroes`。主文件为 `build.ps1`；运行说明位于 `README.md`。

针对双击 `build\PocketHeroes.exe` 时额外出现命令提示符的问题，链接参数加入 `-mwindows`，生成 Windows 图形子系统程序。保留现有 SFML 3.1.0、MSYS2 g++、DLL 复制及输出目录约定。

开始检查 `main`、远端和工作区；`git pull --ff-only origin main` 因 GitHub 网络连接重置失败，改用 GitHub 连接器核对远端 `build.ps1` 的 Git blob SHA，与本地一致。其他会话修改的英雄动作文档及未跟踪资源未纳入本次任务。

验证：`build.ps1` 编译成功；`objdump -p build\PocketHeroes.exe` 显示 `Subsystem 00000002 (Windows GUI)`；程序启动两秒后仍正常运行。未通过人工观察截图验证窗口交互。推送结果以本次 Git 命令核对为准；若网络仍不可用，须明确标注仅本地完成。
