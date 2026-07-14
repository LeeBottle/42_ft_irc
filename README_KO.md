*This project has been created as part of the 42 curriculum by byeolee, hugim, sanghul2*

# Description
본 프로젝트는 C++ 98 표준을 준수하여 구현한 IRC(Internet Relay Chat) 서버입니다. 외부 IRC 클라이언트가 본 서버에 접속하여 실체적인 채팅 서비스를 이용할 수 있도록 TCP/IP(v4 또는 v6) 통신을 기반으로 작동합니다. 다른 서버와의 연동(Server-to-Server) 기능을 배제하고, 오직 복수의 클라이언트 요청을 효율적으로 처리하는 단일 서버 아키텍처를 목표로 합니다.

## 프로젝트 목표 및 주요 특징
### Non-blocking I/O & 단일 Multiplexing
fork()를 통한 프로세스 생성을 엄격히 금지하며, 전체 성능 저하 및 자원 낭비를 방지하기 위해 모든 소켓과 파일 디렉토리(FD)를 Non-blocking 모드로 제어합니다. 단 하나의 poll() (또는 select, kqueue, epoll 등 동등한 함수)만을 사용하여 listen, read, write 등의 모든 I/O 이벤트를 중앙 집중식으로 동시 처리합니다.
### 데이터 무결성 보장
저대역폭 환경이나 네트워크 지연으로 인해 명령어가 여러 패킷으로 쪼개져 들어오는 상황(Partial Data)을 완벽히 상정하여 구현했습니다. 수신된 데이터 조각들을 각 클라이언트별 버퍼에 온전히 취합(Aggregate)한 후, 완성된 명령어 단위(\n 또는 \r\n)로 파싱하여 실행합니다.
### IRC 프로토콜 명세 일부 구현
공식 IRC 프로토콜을 참고하여 참조 클라이언트(Reference Client)와의 연결 오류가 없도록 설계되었으며, 채팅 서버로서 필수적인 아래 기능들을 핵심적으로 제공합니다.

## 지원 기능 구체안
### 사용자 관리 및 인증
서버 접속 비밀번호 검증, 유저 인증, 닉네임(Nickname) 및 유저네임(Username) 설정, 개인 메시지(1:1 DM) 송수신.
### 채널(방) 시스템
채널 생성 및 입장(JOIN), 특정 채널에 속한 모든 유저에게 메시지 브로드캐스팅(Forwarding).
### 권한 분리
채널 운영자(Operator)와 일반 유저(Regular User) 등급 분리.
### 운영자 전용 명령어
- KICK: 특정 유저를 채널에서 강제 퇴장
- INVITE: 특정 유저를 채널에 초대
- TOPIC: 채널의 주제 변경 및 조회
- MODE: 채널 모드 변경 기능 지원
	- i (Invite-only): 초대받은 유저만 입장 가능하도록 제한/해제
	- t (Topic restriction): TOPIC 명령어 사용 권한을 운영자 제한으로 설정/해제
	- k (Channel key): 채널 입장 비밀번호 설정/해제
	- o (Operator privilege): 타 유저에게 운영자 권한 부여/회수
	- l (User limit): 채널 최대 입장 인원 제한/해제





# Instructions
## 1. Prerequisites
본 프로젝트는 C++ 98 표준을 기반으로 작성되었으며 정상적인 빌드와 실행 및 테스트를 위해 아래와 같은 환경과 도구들이 시스템에 준비되어야 함
- 컴파일 및 빌드 도구
	- GNU Make 유틸리티
	- C++ 98 표준을 지원하는 c++ 또는 g++ 컴파일러
- 네트워크 테스트용 클라이언트 프로그램
	- 패킷 분할 전송(Partial Data) 및 소켓 연결 테스트를 위한 nc (netcat) 유틸리티
	- 명령어 호환성 및 실제 IRC 통신 규격 검증을 위한 표준 IRC 참조 클라이언트 (Irssi)

## 2. Compilation
리포지토리 루트 경로에서 Makefile에 정의된 다음 명령어들을 사용
- 빌드 명령어
	```
	make
	```

	- Makefile에 정의된 규칙에 따라 소스 코드를 컴파일하고 `ircserv` 실행 파일 생성

	```
	make all
	```
	- make 명령어와 동일하게 전체 프로젝트 빌드 수행

- 정리 및 재빌드 명령어
	```
	make clean
	```
	- 컴파일 과정에서 생성된 오브젝트 파일(.o) 삭제

	```
	make fclean
	```
	- 오브젝트 파일과 함께 생성된 ircserv 실행 파일까지 모두 삭제

	```
	make re
	```
	- 이전 빌드 결과물을 완전히 삭제(fclean)한 후 처음부터 다시 컴파일(make) 진행

## 3. Execution
컴파일이 완료된 후 프로그램을 실행하는 명령어 형식과 인자
- 실행 명령어 형식
	```
	./ircserv <port> <password>
	```
- Arguments 구성
	- port: IRC 서버가 연결 요청을 대기할 포트 번호
	- password: 서버 접속 시 클라이언트 인증에 필요한 비밀번호
- 주요 예외 처리 항목
	- 인자 개수 오류: 프로그램 실행 시 입력된 인자가 부족하거나 초과된 경우 에러 메시지 출력 후 종료
	- 포트 번호 유효성: 포트 범위를 벗어난 입력(0 미만 또는 65535 초과)이나 숫자가 아닌 문자가 포함된 경우 예외 처리
	- 소켓 바인딩 실패: 시스템에서 이미 사용 중인 포트이거나 권한이 없는 포트 번호 접근 시 실행 차단
	- 시스템 콜 에러: 소켓 생성, Non-blocking 설정(fcntl), listen 등록 실패 시 자원 해제 후 안전하게 종료

## 4. Testing
### irssi
표준 IRC 참조 클라이언트
#### 기본 접속 및 인증 테스트
```
/connect <host> <port> <password>
```
- 정상 케이스
	- 비밀번호 일치 시 핸드셰이크 완료 및 유저/닉네임 등록 성공
- 예외 및 오류 처리
	- 잘못된 비밀번호 입력 시 서버에서 연결을 승인하지 않고 ERR_PASSWDMISMATCH (464) 반환 후 접속 차단
	- 이미 서버에 존재하는 닉네임으로 접속 시도 시 ERR_NICKNAMEINUSE (433) 반환 및 닉네임 변경 요구
#### 사용자 관리 및 인증
```
/join #<channel>
```
- 정상 케이스
	- 채널 생성 또는 입장 성공 후 메시지 브로드캐스팅 정상 작동
- 예외 및 오류 처리
	- 유효하지 않은 채널 이름 형식(예: # 없이 입력)으로 접근 시 ERR_NOSUCHCHANNEL (403) 반환
	- 채널에 가입되지 않은 유저가 해당 채널로 메시지 전송 시도 시 ERR_CANNOTSENDTOCHAN (404) 반환

```
/part #<channel>
```
- 정상 케이스
	- 해당 채널에서 정상적으로 퇴장하며, 채널 내 다른 유저들에게 퇴장 메시지가 브로드캐스팅
- 예외 및 오류 처리
	- 대상 채널이 존재하지 않는 경우: ERR_NOSUCHCHANNEL (403) 반환
	- 채널은 존재하나 본인이 해당 채널에 가입되어 있지 않은 경우: ERR_NOTONCHANNEL (442) 반환
	- 명령어 인자(채널명)가 누락된 경우: ERR_NEEDMOREPARAMS (461) 반환

```
/msg <nickname> <message>
```
- 정상 케이스
	- 대상 유저의 개별 창에 메시지가 정상적으로 표시되며 1:1 DM이 성립
- 예외 및 오류 처리
	- 메시지를 보낼 대상(닉네임)이 서버에 존재하지 않는 경우: ERR_NOSUCHNICK (401) 반환
	- 메시지 내용이 비어있거나 수신자가 지정되지 않은 경우: ERR_NORECIPIENT (411) 또는 ERR_NOTEXTTOSEND (412) 반환

```
/nick <new_nickname>
```
- 정상 케이스
	- 현재 사용 중인 닉네임이 새로운 닉네임으로 변경되며, 참여 중인 채널의 다른 유저들에게 변경 사실이 알림 처리
- 예외 및 오류 처리
	- 변경하려는 닉네임이 이미 서버의 다른 유저에 의해 사용 중인 경우: ERR_NICKNAMEINUSE (433) 반환
	- 닉네임에 사용할 수 없는 잘못된 문자(예: 공백, 사용할 수 없는 특수문자 등)가 포함된 경우: ERR_ERRONEUSNICKNAME (432) 반환
	- 명령어 인자(새로운 닉네임)가 누락된 경우: ERR_NONICKNAMEGIVEN (431) 반환

#### 채널 운영자(Operator) 전용 명령어 테스트 (KICK, INVITE, TOPIC)
- 정상 운영자 케이스
```
/kick #<channel> <user_name>
/invite <user_name> #<channel>
/topic #<channel> <new_topic>
```
- 예외 및 오류 처리 (권한 부족)
	- 일반 유저가 위 명령어들을 시도할 경우 ERR_CHANOPRIVSNEEDED (482) 에러 응답 수신 확인
- 예외 및 오류 처리 (대상 부재)
	- 존재하지 않는 유저를 KICK 또는 INVITE 할 경우 ERR_NOSUCHNICK (401) 반환
- 예외 및 오류 처리 (채널 부재)
	- 존재하지 않는 채널을 대상으로 명령 수행 시 ERR_NOSUCHCHANNEL (403) 반환
- 예외 및 오류 처리 (KICK 대상 부재)
	- 채널에 존재하지 않는 유저를 강퇴하려 할 경우 ERR_USERNOTINCHANNEL (441) 반환
- 예외 및 오류 처리 (INVITE 중복)
	- 이미 채널에 가입되어 있는 유저를 초대할 경우 ERR_USERONCHANNEL (443) 반환
#### 채널 모드(MODE) 변경 및 규칙 위반 테스트

-	초대 전용 모드
	```
	/mode #<channel> +i
	/mode #<channel> -i
	```
	- 정상 케이스: 운영자가 설정 후 초대받은 유저만 채널 입장 허용
	- 예외 및 오류 처리: +i 상태인 채널에 초대(INVITE)받지 않은 일반 유저가 진입 시도 시 ERR_INVITEONLYCHAN (473) 반환


- 주제 변경 제한 모드
	```
	/mode #<channel> +t
	/mode #<channel> -t
	```
	- 정상 케이스: +t 설정 시 운영자만 채널 주제 수정 가능
	- 예외 및 오류 처리: +t 상태에서 권한이 없는 일반 유저가 /topic으로 주제 변경 시도 시 ERR_CHANOPRIVSNEEDED (482) 반환


- 채널 비밀번호 모드
	```
	/mode #<channel> +k <password>
	/mode #<channel> -k 
	```
	- 정상 케이스: 올바른 비밀번호를 입력한 유저만 입장 허용
	- 예외 및 오류 처리: 비밀번호가 설정된 채널에 틀린 비밀번호로 입장 시도 시 ERR_BADCHANNELKEY (475) 반환


- 인원 제한 모드
	```
	/mode #<channel> +l <number>
	/mode #<channel> -l 
	```
	- 정상 케이스: 채널 내 동시 접속자 수를 최대 <number>명으로 제한
	- 예외 및 오류 처리: 정원이 가득 찬(+l 정원 초과) 채널에 추가로 입장 시도 시 ERR_CHANNELISFULL (471) 반환

- 운영자 권한 부여/회수
	```
	/mode #ft_irc +o <user_name>
	/mode #ft_irc -o <user_name>
	```
	- 정상 케이스: 타 유저에게 운영자 등급을 양도하거나 회수 가능
	- 예외 및 오류 처리: 채널 내에 존재하지 않는 유저에게 권한을 부여하려 할 경우 ERR_USERNOTINCHANNEL (441) 반환


### nc
수동 명령어 입력 및 로우 데이터(Raw Data) 검증 클라이언트
#### 기본 접속 및 인증 테스트
```
nc -C <host> <port> 
PASS <password>, NICK <nick>, USER <user> 0 * :user
```
- 정상 케이스
	- 비밀번호 일치 시 핸드셰이크 완료 및 001 Welcome 메시지 수신 성공
- 예외 및 오류 처리
	- 잘못된 비밀번호 입력 후 NICK/USER 전송 시 서버에서 연결을 승인하지 않고 ERR_PASSWDMISMATCH (464) 반환 후 접속 차단
	- 이미 서버에 존재하는 닉네임 전송 시도 시 ERR_NICKNAMEINUSE (433) 반환 및 새로운 닉네임 재입력 요구
#### 채널 입장 및 메시지 전송 테스트
```
JOIN #<channel>
PRIVMSG #<channel> :<message>
```
- 정상 케이스
	- 채널 생성 또는 입장 성공 후 해당 채널로의 PRIVMSG 데이터 브로드캐스팅 수신 정상 작동
- 예외 및 오류 처리
	- 유효하지 않은 채널 이름 형식(예: # 없이 JOIN ft_irc 입력)으로 접근 시 ERR_NOSUCHCHANNEL (403) 반환
	- 채널에 가입되지 않은 유저가 해당 채널로 PRIVMSG 전송 시도 시 ERR_CANNOTSENDTOCHAN (404) 반환

```
PART #<channel>
```
- 정상 케이스
	- 대상 유저의 개별 창에 메시지가 정상적으로 표시되며 1:1 DM이 성립
- 예외 및 오류 처리
	- 메시지를 보낼 대상(닉네임)이 서버에 존재하지 않는 경우: ERR_NOSUCHNICK (401) 반환
	- 메시지 내용이 비어있거나 수신자가 지정되지 않은 경우: ERR_NORECIPIENT (411) 또는 ERR_NOTEXTTOSEND (412) 반환

```
PRIVMSG <nickname> :<message>
```
- 정상 케이스
	- 대대상 유저에게 원시 데이터(Raw Data) 형태의 PRIVMSG 메시지가 전송
- 예외 및 오류 처리
	- 메시지를 보낼 대상(닉네임)이 서버에 존재하지 않는 경우: ERR_NOSUCHNICK (401) 반환
	- 메시지 내용이 비어있거나 수신자가 지정되지 않은 경우: ERR_NORECIPIENT (411) 또는 ERR_NOTEXTTOSEND (412) 반환

```
NICK <new_nickname>
```
- 정상 케이스
	- 현재 사용 중인 닉네임이 새로운 닉네임으로 변경되며, 참여 중인 채널의 다른 유저들에게 변경 사실이 알림 처리
- 예외 및 오류 처리
	- 변경하려는 닉네임이 이미 서버의 다른 유저에 의해 사용 중인 경우: ERR_NICKNAMEINUSE (433) 반환
	- 닉네임에 사용할 수 없는 잘못된 문자(예: 공백, 사용할 수 없는 특수문자 등)가 포함된 경우: ERR_ERRONEUSNICKNAME (432) 반환
	- 명령어 인자(새로운 닉네임)가 누락된 경우: ERR_NONICKNAMEGIVEN (431) 반환



#### 채널 운영자(Operator) 전용 명령어 테스트 (KICK, INVITE, TOPIC)
- 정상 운영자 케이스
```
KICK #<channel> <user>
INVITE <user_name> #<channel>
TOPIC #<channel> <new_topic>
```
- 예외 및 오류 처리 (권한 부족)
	- 일반 유저가 위 명령어들을 시도할 경우 ERR_CHANOPRIVSNEEDED (482) 에러 응답 수신 확인
- 예외 및 오류 처리 (대상 부재)
	- 존재하지 않는 유저를 KICK 또는 INVITE 할 경우 ERR_NOSUCHNICK (401) 반환
- 예외 및 오류 처리 (채널 부재)
	- 존재하지 않는 채널을 대상으로 명령 수행 시 ERR_NOSUCHCHANNEL (403) 반환
- 예외 및 오류 처리 (KICK 대상 부재)
	- 채널에 존재하지 않는 유저를 강퇴하려 할 경우 ERR_USERNOTINCHANNEL (441) 반환
- 예외 및 오류 처리 (INVITE 중복)
	- 이미 채널에 가입되어 있는 유저를 초대할 경우 ERR_USERONCHANNEL (443) 반환
#### 채널 모드(MODE) 변경 및 규칙 위반 테스트
- 초대 전용 모드
	```
	MODE #<channel> +i
	MODE #<channel> -i
	```
	- 정상 케이스: 운영자가 설정 후 초대받은 유저만 채널 입장 허용
	- 예외 및 오류 처리: +i 상태인 채널에 INVITE 받지 않은 일반 유저가 JOIN 시도 시 ERR_INVITEONLYCHAN (473) 반환


- 주제 변경 제한 모드
	```
	MODE #<channel> +t
	MODE #<channel> -t
	```
	- 정상 케이스: +t 설정 시 운영자만 채널 주제 수정 가능
	- 예외 및 오류 처리: +t 상태에서 권한이 없는 일반 유저가 TOPIC으로 주제 변경 시도 시 ERR_CHANOPRIVSNEEDED (482) 반환


- 채널 비밀번호 모드
	```
	MODE #<channel> +k <password>>
	MODE #<channel> -k 
	```
	- 정상 케이스: 올바른 비밀번호를 입력한 유저만 입장 허용
	- 예외 및 오류 처리: 비밀번호가 설정된 채널에 틀린 비밀번호 또는 비밀번호 없이 입장 시도 시 ERR_BADCHANNELKEY (475) 반환

- 인원 제한 모드
	```
	MODE #<channel> +l <number>
	MODE #<channel> -l 
	```
	- 정상 케이스: 채널 내 동시 접속자 수를 최대 <number>명으로 제한
	- 예외 및 오류 처리: 정원이 가득 찬(+l 정원 초과) 채널에 추가로 JOIN 시도 시 ERR_CHANNELISFULL (471) 반환


- 운영자 권한 부여/회수
	```
	MODE #<channel> +o <user_name>
	MODE #<channel> -o <user_name>
	```
	- 정상 케이스: 타 유저에게 운영자 등급을 양도하거나 회수 가능
	- 예외 및 오류 처리: 채널 내에 존재하지 않는 유저에게 권한을 부여하려 할 경우 ERR_USERNOTINCHANNEL (441) 반환

## 5. Network Edge Cases & Advanced Handling
클라이언트 비정상 종료 대응
- 유저가 ctrl+C 등으로 소켓을 강제 차단하거나 네트워크 단절 시 poll() 이벤트 루프에서 이를 감지하여 소켓을 안전하게 close
- 유저가 가입해 있던 모든 채널에서 자동 이탈(QUIT) 처리하여 무결성 유지
비정상적 페이로드 방어
- 개행 문자(\n)가 명시되지 않은 대용량 악성 데이터가 무한히 들어오거나 비정상적인 버퍼 오버플로우 공격 시도가 있을 때
- 일정 바이트 규격을 초과하는 패킷
- 유효하지 않은 명령어로 간주해 버퍼를 비우거나 연결을 강제 드롭하는 예외 처리


# Resources

## IRC protocol references

| 주제                           | 자료                                                                                                | 용도                                                                                                                          |
| ---------------------------- | ------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| IRC 원본 프로토콜                  | [RFC 1459 - Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html)            | IRC message 형식, command, numeric reply의 기본 기준                                                                               |
| IRC 원본 프로토콜 (IETF)           | [RFC 1459 - Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)          | IRC 프로토콜의 표준 아키텍처와 기본 메시지 포맷, 클라이언트-서버 간의 통신 규약을 이해하기 위해 참조. KICK, INVITE, TOPIC, MODE 등의 기본 동작과 Numeric Replies의 표준 규격을 학습 |
| IRC architecture             | [RFC 2810 - Internet Relay Chat: Architecture](https://www.rfc-editor.org/rfc/rfc2810.html)       | IRC client/server/channel 구조 이해용                                                                                            |
| IRC channel management       | [RFC 2811 - Internet Relay Chat: Channel Management](https://www.rfc-editor.org/rfc/rfc2811.html) | channel, channel mode, operator, invite-only 개념 확인                                                                          |
| IRC client protocol          | [RFC 2812 - Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/rfc/rfc2812.html)    | client-server command 형식, PASS/NICK/USER/JOIN/PRIVMSG/MODE 등 확인                                                             |
| IRC server protocol          | [RFC 2813 - Internet Relay Chat: Server Protocol](https://www.rfc-editor.org/rfc/rfc2813.html)    | server-to-server 참고용. ft_irc mandatory 구현 대상은 아님                                                                            |
| Modern IRC specification     | [Modern IRC Client Protocol](https://modern.ircdocs.horse/)                                       | RFC 1459 이후 현대적인 IRC 클라이언트들이 사용하는 프로토콜 명세와 차이점을 파악하기 위해 참조                                                                  |
| IRCv3 capability negotiation | [IRCv3 - Capability Negotiation](https://ircv3.net/specs/extensions/capability-negotiation.html)  | irssi가 접속 초기에 보내는 `CAP LS`, `CAP END` 이해용                                                                                   |
|Irssi Chat Client| [Irssi](https://irssi.org/documentation/settings/)| IRSSI 공식 사이트 setting 설정 확인 |



## Linux / POSIX system call references

| 분류                      | 함수                                                                             | 자료                                                                                                                                                                                               |
| ----------------------- | ------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| socket 생성               | `socket()`                                                                     | [Linux man page - socket(2)](https://man7.org/linux/man-pages/man2/socket.2.html)                                                                                                                |
| socket option           | `setsockopt()`                                                                 | [Linux man page - setsockopt(2)](https://man7.org/linux/man-pages/man2/setsockopt.2.html)                                                                                                        |
| bind/listen/accept      | `bind()`, `listen()`, `accept()`                                               | [bind(2)](https://man7.org/linux/man-pages/man2/bind.2.html), [listen(2)](https://man7.org/linux/man-pages/man2/listen.2.html), [accept(2)](https://man7.org/linux/man-pages/man2/accept.2.html) |
| client connect          | `connect()`                                                                    | [Linux man page - connect(2)](https://man7.org/linux/man-pages/man2/connect.2.html)                                                                                                              |
| socket address 확인       | `getsockname()`                                                                | [Linux man page - getsockname(2)](https://man7.org/linux/man-pages/man2/getsockname.2.html)                                                                                                      |
| 주소 확인                   | `getaddrinfo()`, `freeaddrinfo()`                                              | [Linux man page - getaddrinfo(3)](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)                                                                                                      |
| 주소 확인, legacy           | `gethostbyname()`                                                              | [Linux man page - gethostbyname(3)](https://man7.org/linux/man-pages/man3/gethostbyname.3.html)                                                                                                  |
| protocol DB             | `getprotobyname()`                                                             | [Linux man page - getprotoent(3)](https://man7.org/linux/man-pages/man3/getprotoent.3.html)                                                                                                      |
| byte order 변환           | `htons()`, `htonl()`, `ntohs()`, `ntohl()`                                     | [Linux man page - byteorder(3)](https://man7.org/linux/man-pages/man3/htons.3.html)                                                                                                              |
| IP 주소 변환                | `inet_addr()`, `inet_ntoa()`, `inet_ntop()`                                    | [inet(3)](https://man7.org/linux/man-pages/man3/inet.3.html), [inet_ntop(3)](https://man7.org/linux/man-pages/man3/inet_ntop.3.html)                                                             |
| data receive            | `recv()`                                                                       | [Linux man page - recv(2)](https://man7.org/linux/man-pages/man2/recv.2.html)                                                                                                                    |
| data send               | `send()`                                                                       | [Linux man page - send(2)](https://man7.org/linux/man-pages/man2/send.2.html)                                                                                                                    |
| event multiplexing      | `poll()`                                                                       | [Linux man page - poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html)                                                                                                                    |
| non-blocking fd 설정      | `fcntl()`                                                                      | [Linux man page - fcntl(2)](https://man7.org/linux/man-pages/man2/fcntl.2.html)                                                                                                                  |
| signal 처리               | `signal()`, `sigaction()`                                                      | [signal(2)](https://man7.org/linux/man-pages/man2/signal.2.html), [sigaction(2)](https://man7.org/linux/man-pages/man2/sigaction.2.html)                                                         |
| signal set 조작           | `sigemptyset()`, `sigfillset()`, `sigaddset()`, `sigdelset()`, `sigismember()` | [Linux man page - sigsetops(3)](https://man7.org/linux/man-pages/man3/sigsetops.3.html)                                                                                                          |
| fd 닫기                   | `close()`                                                                      | [Linux man page - close(2)](https://man7.org/linux/man-pages/man2/close.2.html)                                                                                                                  |
| file offset / file info | `lseek()`, `fstat()`                                                           | [lseek(2)](https://man7.org/linux/man-pages/man2/lseek.2.html), [fstat(2)](https://man7.org/linux/man-pages/man2/fstat.2.html)                                                                   |

## Additional references

| 주제                  | 자료                                                                             | 용도                                                                                                                                      |
| ------------------- | ------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------- |
| Network Programming | [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/) | C/C++ 기반의 소켓 프로그래밍과 TCP/IP 네트워크의 기초를 이해하고 `socket()`, `bind()`, `listen()`, `accept()`, `recv()`, `send()`, `poll()` 등의 사용법을 학습하기 위해 참조 |



## Use of AI
### 번역
본 프로젝트의 최종 README.md 문서를 한국어에서 영어로 번역하고 문맥을 매끄럽게 다듬는 과정에 생성형 AI를 활용


