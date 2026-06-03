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

# 05/28
## handle_command_pass
클라이언트가 처음 보낸 비밀번호가 서버 설정 비밀번호와 일치하는지 검증  

## handle_command_nick
닉네임을 설정  
공백이 섞여 들어와도 첫 번째 인자만 사용  
중복 검사 기능  
```
./irssi  
/connect <ipaddress> <port> <password>  
/NICK <text>  
```

## handle_command_user
계정명 설정  
서버 내 중복이 가능하므로 중복 검사를 하지 않음  
```
./irssi  
/connect <ipaddress> <port> <password>  
/USER <text>  
```

## handle_command_privmsg
클라이언트가 보낸 대화 패킷에서 콜론(:) 또는 공백 뒤에 오는 순수한 대화 텍스트 본문만 사용하는 함수  
->irssi에서 /PASS 1234가 아니라 PASS 1234의 경우 텍스트 처리되어야 함  

## irssi에서 /quote가 없을 시
대화방이 없는 메인 로비
/JOIN으로 강제로 대화방에 갈 경우 /quote없이도 입력 가능
->nc에서는 /JOIN없이도 대화가 가능한상황
->채팅방 기능 추가 필요



# 06/03

## 서버 구동
./ircserv <port> <password>

## irssi
/connect <ip> <port> <password>

 - 예외처리
	- 비밀번호 누락 (/connect  <ip> <port>)
	- 비밀번호 불일치 (/connect  <ip> <port> <wrong password>)

/msg <message>

/nick <newnick>

/join #lobby
<message>
/part #lobby <message>



## nc
nc -C <ip> <port> 
PASS <password>
NICK <nickname>
USER <username>
JOIN #lobby
PRIVMSG #lobby :<message>
PART #lobby :<message>


# 추가 사항
## 운영자 전용 필수 명령어
	KICK
	INVITE
	TOPIC
	MODE
## MODE 세부 플래그