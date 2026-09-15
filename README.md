# OpenKHPanaceaForChinesePatch

基于 [OpenKH](https://github.com/OpenKH/OpenKh) commit `7a3b945c538d32c6a285128c98aefba093f52ceb` 修改而来

## 新增功能：
* 1.5+2.5的Launcher可以读取mod文件
* 2.8的Launcher可以读取mod文件
* KH1 Theater可以读取mod文件
* 增加了OpenMovie的Hook，可以把视频文件重定向到mod里
* 所有游戏的设置菜单都可以读取mod文件
* KH1字体扩容hook（DI版DLL限定）
* KH1 exe文本汉化（DI版DLL限定）
* 1.5+2.5 Launcher exe文本汉化（DI版DLL限定）
* KH1 Theater exe文本汉化（DI版DLL限定）

## 与原版Panacea的不同点：
* 默认移除了VAG_STREAM相关的hook，不再需要dependencies目录（可以通过开启cmake option的PANACEA_WITH_VAG_STREAM_HOOKS将这个功能增加回来）

## 注意事项
* 汉化注入相关部分只支持steam版

## 关于OpenMovie hook
* 因为Epic版的日版视频路径跟Steam版的日版不太一样，Epic版会用jp路径，Steam版会用dt路径，为了方便补丁制作所以当检测到运行的是Epic版的时候，会把路径里的jp文件夹全部替换成dt