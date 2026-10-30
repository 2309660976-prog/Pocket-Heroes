# GitHub 协作交接

## 范围

项目：`E:\Pocket Heroes`。涉及 Git 仓库、`README.md`、`docs/协作指南.md` 和 GitHub 发布，不涉及游戏逻辑或剧情设计。

## 当前状态

本地 `main` 分支包含已完成的 SFML 教学原型及用户后续拆分的 `Game`、`Hero`、`Monster` 源码、剧情文档和三张角色图片。目标仓库为个人账号 `2309660976-prog` 下的公开仓库 `Pocket-Heroes`。

仓库已由 GitHub Desktop 发布到 `https://github.com/2309660976-prog/Pocket-Heroes`，公开，默认分支为 `main`。本地 `origin` 指向该地址；本次通过 `git pull --ff-only origin main` 确认与远端同步。项目根目录 `AGENTS.md` 规定每次实际任务开始拉取、完成后提交并上传，只提交本次负责的文件。

## 验证与待办

- `build.ps1` 已使用本机 SFML 3.1.0 和 MSYS2 g++ 编译成功。
- `git diff --check` 无空白错误；`build/` 被 `.gitignore` 排除。
- 本次改动为根目录 `AGENTS.md` 和本交接记录；未跟踪的地图模块文件属于其他会话，未纳入本次提交。
- 之后确认朋友是否需要直接写入权限；若需要，由所有者邀请其 GitHub 用户名。
- 朋友协作步骤见 `docs/协作指南.md`。新会话须重新核实 Git 状态、远端地址和实际上传结果。
