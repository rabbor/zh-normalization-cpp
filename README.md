# zh-normalization-cpp
This code uses Google/Sparrowhawk to execute a far file generated based on the OpenGrm Thrax syntax to normalize Chinese text, serving as a pre-processing step for tokenization in text-to-speech processing. It uses only C++ and has no other dependencies.
使用google/Sparrowhawk 执行 基于 OpenGrm Thrax 语法生成的 far 文件来实现 中文文本归一化，用作 文本到语音处理中token 化的前一步操作。仅使用 c++，无其他依赖
## 示例：
* 输入：共465篇，约315万字。共计6.42万人，总量的1/5以上。相当于头发丝的1/16， 同比增长6.3%。日期2002/01/28，或2002.01.28，8月16号12:00之前。我是5:02开始的？于5:35:36发射。8:00 a.m.准时开会！湖人 78:96 勇士。计算-2的绝对值是2！ ±2的平方都是4。价格是￥13.5，美国计算是价格是$13.5。最高气温38°C，速度是10km/h！可以拨打12306来咨询。我存了 1000 万。我儿子喜欢这地儿.他说:"你好呀，我很开心呢！"。2026 年 3 月 31 日，安全研究员 Chaofan Shou (@Fried_rice) 发现 Anthropic 的 npm 包中暴露了 .map 文件，别把 .env、.npmrc、.gitignore 提交上去。仓库地址是 https://github.com/instructkr/claude-code，请模仿 {whisper} 的语气说“别出声."，〖重磅〗《新品发布》——现在开始！请求接入 -> 身份与策略判定 -> 域服务处理. 联系邮箱：ops+tts@example.ai。fabric-api-0.91.3+1.20.2.jar 需要单独下载。

* 输出：共四百六十五篇，约三百一十五万字。共计六点四二万人，总量的五分之一以上。相当于头发丝的十六分之一，同比增长百分之六点三。日期二零零二年一月二十八日，或二零零二年一月二十八日，八月十六号十二点之前。我是五点零二分开始的？于五点三十五分三十六秒发射。八点 a.m. 准时开会！湖人七八比九六勇士。计算负二的绝对值是二！正负二的平方都是四。价格是十三点五元，美国计算是价格是十三点五美元。最高气温三十八摄氏度，速度是 10km/h！可以拨打幺二三零六来咨询。我存了一千万。我儿子喜欢这地儿. 他说 :" 你好呀，我很开心呢！"。两千零二十六年三月三十一日，安全研究员 Chaofan Shou( @Fried_rice) 发现 Anthropic 的 npm 包中暴露了 .map 文件，别把 .env、.npmrc、.gitignore 提交上去。仓库地址是 https://github.com/instructkr/claude-code，请模仿 " whisper" 的语气说“别出声."，" 重磅 " 《新品发布》。现在开始！请求接入，身份与策略判定，域服务处理. 联系邮箱：ops+tts@example.ai。fabric-api-0.91.3+1.20.2.jar 需要单独下载。


## 说明

```text
src: 归一化代码，依赖sparrowhawk
third：mac arm 系统上编译的第三放依赖库
zh-normalizer-grm: OpenGram Thrax syntax 文件，以及sparrowhawk启动的 proto 配置文件
```

## 第三方编译注意事项：
* 版本：
```text
abseil： abseil-cpp-20250814.2
openfst：openfst-1.6.7   编译中有一个报错，很好修复
protobuf： protobuf-25.0  使用旧的版本
re：re2-2025-11-05 
thrax： thrax-1.2.8   配合openfst-1.6.7，有兼容性要求，这两个版本我验证过，编译中间有一个小错误，可以手动修复，很简单
sparrowhawk： 最新的版本，修改了 src/proto/semiotic_classes.proto 文件，将时间部分字段改成字符串，然后编译
```

* sparrowhawk 变更的 semiotic_classes.proto 文件
- 原始
```text
message Time {
  optional /* required */ int32 hours = 1;
  optional /* required */ int32 minutes = 2;
  optional int32 seconds = 3 [deprecated = true];
```
- 修改后
```text
message Time {
  optional /* required */ string hours = 1;
  optional /* required */ string minutes = 2;
  optional string seconds = 3 [deprecated = true];
```
## 项目编译
### 1. 编译 生成 far 文件
```bash
cd zh-normalizer-grm
sh build_far.sh
```
### 2. 编译程序
```text
mkdir build
cd  build
cmake ..
make 
```
## 程序运行

* 注意：
如果直接使用当前代码库中的 dylib，需要手动修改 dylib 中的 install_name 路径和 rpath 信息

*  修改 sparrowhawk_configuration.ascii_proto中的sentence_boundary_exceptions_file字段为绝对路径
"{yourProjectDir}/zh-normalizer-grm/sentence_boundary_exceptions.txt"

```bash
cd build
./zh-normalize --config=sparrowhawk_configuration.ascii_proto --path_prefix={project_dir}/zh-normalizer-grm/
```

## 参考：
* 部分 grm 的实现 参考自：[weTextProcessing](https://github.com/wenet-e2e/WeTextProcessing)
