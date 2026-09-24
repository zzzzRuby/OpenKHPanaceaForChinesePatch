# OpenKHPanaceaForChinesePatch

基于 [OpenKH](https://github.com/OpenKH/OpenKh) commit `7a3b945c538d32c6a285128c98aefba093f52ceb` 修改而来。
* 对于原版 OpenKH.Research.Panacea 所作的功能性增强，基于 Apache 2.0 协议开源，参考 LICENSE 文件。
* 对于新增的 `zh_cn_inject` 部分（即 `src/zh_cn_inject` 目录下的文件），基于 GPL v3 开源，参考 LICENSE.zh_cn_inject 文件。

## 新增功能
* 1.5+2.5 的 Launcher 可以读取 mod 文件
* 2.8 的 Launcher 可以读取 mod 文件
* KH1 Theater 可以读取 mod 文件
* 增加了 OpenMovie 的 Hook，可以把视频文件重定向到 mod 里
* 所有游戏的设置菜单都可以读取 mod 文件
* KH1 字体扩容 hook（DI 版 DLL 限定）
* KH1 exe 文本汉化（DI 版 DLL 限定）
* 1.5+2.5 Launcher exe 文本汉化（DI 版 DLL 限定）
* KH1 Theater exe 文本汉化（DI 版 DLL 限定）

## 关于编译
* **编译命令：** Windows 下运行 `cmake <仓库根目录>`，然后 `cmake --build . --config Release` 即可编译。
* **环境要求：** 需要安装 Windows SDK 和可用的编译器（例如 `clang-cl` 或 `msvc`）。
* **编译产物：**
  * `bin/Release/DBGHELP.dll`（不带汉化注入功能，基于 Apache 2.0 协议开源）
  * `bin/Release/DI/DBGHELP.dll`（带有汉化注入功能，基于 GPL v3 开源）

## 与原版 Panacea 的不同点
* 默认移除了 `VAG_STREAM` 相关的 hook，不再需要 `dependencies` 目录。
* *提示：可以通过开启 CMake Option 的 `PANACEA_WITH_VAG_STREAM_HOOKS` 将这个功能增加回来。*

## 注意事项
* 汉化注入相关部分只支持 Steam 版和 Epic 学习版，Epic 正版因为我没有所以没有做，是根据 exe 的 SHA512 来确定版本的。

## 关于 OpenMovie hook
* Epic 版的日版视频路径跟 Steam 版的日版不太一样，Epic 版会用 `jp` 路径，Steam 版会用 `dt` 路径。
* 为了方便补丁制作，所以当检测到运行的是 Epic 版的时候，会把路径里的 `jp` 文件夹全部替换成 `dt`。
