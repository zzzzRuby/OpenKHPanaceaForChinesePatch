# OpenKHPanaceaForChinesePatch

基于 [OpenKH](https://github.com/OpenKH/OpenKh) commit `7a3b945c538d32c6a285128c98aefba093f52ceb` 修改而来
对于原版OpenKH.Reasearch.Panacea所作的功能性增强，基于Apache 2.0协议开源，参考LICENSE文件
对于新增的zh_cn_inject部分（即src/zh_cn_inject目录下的文件），基于GPL v3开源，参考LICENSE.zh_cn_inject文件

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

## 关于编译：
* windows下运行`cmake <仓库根目录>`，然后`cmake --build . --config Release`即可编译，需要安装windows sdk和可用的编译器，比如clang-cl或者msvc
* 编译产物有两个，bin/Release/DBGHELP.dll不带有汉化注入功能，基于Apache 2.0协议开源，bin/Release/DBGHELP.dll带有汉化注入功能，基于GPL v3开源

## 与原版Panacea的不同点：
* 默认移除了VAG_STREAM相关的hook，不再需要dependencies目录（可以通过开启cmake option的PANACEA_WITH_VAG_STREAM_HOOKS将这个功能增加回来）

## 注意事项
* 汉化注入相关部分只支持steam版和epic学习版，epic版因为我没有所以没有做

## 关于OpenMovie hook
* 因为Epic版的日版视频路径跟Steam版的日版不太一样，Epic版会用jp路径，Steam版会用dt路径，为了方便补丁制作所以当检测到运行的是Epic版的时候，会把路径里的jp文件夹全部替换成dt