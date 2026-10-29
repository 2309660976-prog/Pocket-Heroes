# GitHub 协作交接

## 范围

项目：`E:\Pocket Heroes`。涉及 Git 仓库、`README.md`、`docs/协作指南.md` 和 GitHub 发布，不涉及游戏逻辑或剧情设计。

## 当前状态

本地 `main` 分支包含已完成的 SFML 教学原型及用户后续拆分的 `Game`、`Hero`、`Monster` 源码、剧情文档和三张角色图片。目标仓库为个人账号 `2309660976-prog` 下的公开仓库 `Pocket-Heroes`。

本机 Git Credential Manager 可读取 GitHub 凭据，但命令行到 `github.com` 和 `api.github.com` 的 TLS 握手失败；GitHub 连接器可以读取账号和仓库，却没有创建仓库能力。远端仓库尚未创建，代码尚未上传。切勿把本地提交当作已发布。

## 验证与待办

- `build.ps1` 已使用本机 SFML 3.1.0 和 MSYS2 g++ 编译成功。
- `git diff --check` 无空白错误；`build/` 被 `.gitignore` 排除。
- 源码和文档的本地提交准备完毕后，需创建空的公开 GitHub 仓库，不要在网页上初始化 README、.gitignore 或 License。
- 仓库创建后，将 `main` 推送至远端并核对 GitHub 文件清单及默认分支。之后确认朋友是否需要直接写入权限；若需要，由所有者邀请其 GitHub 用户名。
- 朋友协作步骤见 `docs/协作指南.md`。新会话须重新核实 Git 状态、远端地址和实际上传结果。
