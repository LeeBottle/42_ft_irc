# ft_irc
```
./ircserv <port> <password>  
```


## irssi의 경우
```
./irssi  
/connect <ipaddress> <port> <password>  
/quote <text>  
```
사용자가 대화창에 직접 타이핑하기 전에 서버와 소켓 연결이 맺어지자마자 백그라운드에서 CAP LS, PASS <비밀번호>\r\n, NICK, USER 명령어를 자동 전송
이때 여러 명령어가 하나의 TCP 패킷 덩어리로 뭉쳐서 서버의 read() 버퍼에 들어옴

```
/set nick 내닉네임
/set user_name 내유저네임
```
NICK, USER 의 경우 irssi에서 저장된 값이 그대로 전송됨


## nc 경우
```
nc <ipaddress> <port>  
PASS <password>  
```
접속한 직후에는 아무런 데이터도 서버에 먼저 보내지 않고 가만히 연결만 유지
PASS 명령어로 접근 허용 받아야 함
+NICK, USER 명령어도 마찬가지로 작동되어야 함


## irssi에서 quote
/quote 명령어는 필터링 없이 그냥 hello\r\n이라는 문자열을 서버로 보내줌
irssi에서 슬래시 없이 hello라고 치면, 서버 수신 버퍼에서는 아래처럼 받음
```
PRIVMSG * :hello
```

## 목 목표
PRIVMSG처리  
->irssi에서 /PASS 1234가 아니라 PASS 1234의 경우 텍스트 처리되어야 함  
->nc에서 PASS를 또 쳤을 때 서버가 클라이언트를 강퇴하는 것은 틀린것이니 끊지 말고 에러 번호만 응답해야 함