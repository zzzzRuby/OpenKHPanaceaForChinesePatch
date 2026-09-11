# OpenKHPanaceaForChinesePatch

基于 https://github.com commit `7a3b945c538d32c6a285128c98aefba093f52ceb` 修改而来

## 新增功能：
* 1.5+2.5的Launcher可以读取mod文件
* 2.8的Launcher可以读取mod文件
* KH1 Theater可以读取mod文件
* 增加了OpenMovie的Hook，可以把视频文件重定向到mod里
* 所有游戏的设置菜单都可以读取mod文件
* KH1字体扩容hook（DI版DLL限定）

## 与原版Panacea的不同点：
* 默认移除了VAG_STREAM相关的hook，不再需要dependencies目录（可以通过开启cmake option的PANACEA_WITH_VAG_STREAM_HOOKS将这个功能增加回来）
