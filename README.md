# nbtp3

nbtp3是一个借助图床cdn进行数据传输的的通道建立软件。

nbtp3通过图床，将互联网上两个本来应该建立直接连接的点用两个和图床的基于块的连接对接起来，
从而借助图床cdn进行上传/下载加速，这种传输方法会极大的增加延迟，但是也极大的提升了带宽。

nbtp3不能用来充当一般的浏览器代理，虽然它可以做一部分代理的工作。nbtp3可以用来下载http(s)://协议
的文件，或者下载file://协议的文件（位于本机）。

nbtp3的代码宗旨就是高效率和低内存占用。通过三个线程之间的互相联动协作，获得更好的传输效果。
代码上，nbtp3尽量使用C++的最新标准。代码主要目标平台是Linux，理论上可以但是并不保证可以在其他平台稳定编译通过并运行。

# 原理

近似的等同于aria2、wget、curl等等，nbtp3是一个下载软件。但是它的正常运行依赖于另一台也运行了nbtp3的主机。
主机和客户端之间通过某种协议进行通信。客户端把需要连接的目标与连接方式（协议）告知主机，主机即可向目标主动建立连接，
同时主机会把目标发送到主机的所有数据上传到一个开放的文件共享平台上（比如图床）。

借助文件共享平台强大的CDN加速，主机上传到平台的速度和客户端从平台下载的速度都远远快于主机到客户端的上传下载速度。
主机每上传成功一块数据之后，便发给客户端一个链接凭证，客户端凭借凭证去开放平台取回数据，解密或者转码后拼接到文件中，
这就是nbtp3的工作过程以及工作原理。

# 状态

nbtp3正在绝赞开发中，喜欢这个项目的话，如果你想要加入一起贡献代码，欢迎和Neboer取得联系。

# 依赖

nbtp3优先使用cmake进行构建。

nbtp3依赖如下开源项目：lodepng、curlpp、curl。

nbtp3是一个完全开源的程序，目前基于MIT协议。