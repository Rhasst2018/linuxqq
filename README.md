# Linux QQ base on the Qt Creator

## Development environment
* Qt4
* MySql
## Usage
1. Import database into MySql,then open the source code of qqserver, there is a head file named Sqlconnection.h, change the username and password in the file to yourself database username and password, and recompile the qqserver code.
2. First you should run the server and then it's client turn. There is username and password that already register in the database table named usr_info.Or you can click register button to register a new account.
3. It will automatically load the list of friends when login success.Click the Search button to add new friend,the friend you add should be already registed.</br>
	Note: Friends is a one-way added, so your friends should add you too so that you can talk.
4. If your friends is offline,his(her) head portrait is gray and you can't talk with him(her).If friends is online, Only two people have opened the chat window to chat. In face ,friends should automatic pop chat window when receive message, but that code has some problems, the first message is not received, due to time constraints, I delete it.

There are some bugs yet and need to further improve, thanks! 


## 开发环境：
* Qt4
* MySql

## 用法：

1.先将数据库导入到MySql中，然后打开服务器代码，有个名叫Sqlconnection.h的头文件，把里面的用户名和密码改成自己的数据库访问用户名和密码,重新编译即可。

2.运行时先运行服务器，再运行客户端。客户端数据库里的usr_info表里有已经注册过的用户名和密码，也可以自己点击注册按钮，重新注册一个账户。

3.登录成功会自动加载好友列表，点击Search按钮，添加好友,所添加的好友必须时已经注册的
	注意：添加好友是单向添加的，好友也需要再添加你一次，双方才可通信。

4.若好友不在线，按钮便是灰色的，即不能与好友聊天，也不能点击。若好友在线，需两人都打开了聊天窗口，才可聊天。实际上，当好友接收到消息时应该自动弹出聊天窗口，但那段代码有点小问题，第一条消息收不到，由于时间紧迫，便将那段代码删掉了。

程序功能还不是很完善，还需修改，谢谢！