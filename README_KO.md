*This project has been created as part of the 42 curriculum by user1, user2, user3*

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
	- make: Makefile에 정의된 규칙에 따라 소스 코드를 컴파일하고 ircserv 실행 파일 생성
	- make all: make 명령어와 동일하게 전체 프로젝트 빌드 수행
- 정리 및 재빌드 명령어
	- make clean: 컴파일 과정에서 생성된 오브젝트 파일(.o) 삭제
	- make fclean: 오브젝트 파일과 함께 생성된 ircserv 실행 파일까지 모두 삭제
	- make re: 이전 빌드 결과물을 완전히 삭제(fclean)한 후 처음부터 다시 컴파일(make) 진행

## 3. Execution
컴파일이 완료된 후 프로그램을 실행하는 명령어 형식과 인자
- 실행 명령어 형식
	- ./ircserv [port] [password]
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
/connect 127.0.0.1 6667 password
- 정상 케이스
	- 비밀번호 일치 시 핸드셰이크 완료 및 유저/닉네임 등록 성공
- 예외 및 오류 처리
	- 잘못된 비밀번호 입력 시 서버에서 연결을 승인하지 않고 ERR_PASSWDMISMATCH (464) 반환 후 접속 차단
- 예외 및 오류 처리
	- 이미 서버에 존재하는 닉네임으로 접속 시도 시 ERR_NICKNAMEINUSE (433) 반환 및 닉네임 변경 요구
#### 채널 입장 및 메시지 전송 테스트
/join #ft_irc
- 정상 케이스
	- 채널 생성 또는 입장 성공 후 메시지 브로드캐스팅 정상 작동
- 예외 및 오류 처리
	- 유효하지 않은 채널 이름 형식(예: # 없이 입력)으로 접근 시 ERR_NOSUCHCHANNEL (403) 반환
- 예외 및 오류 처리
	- 채널에 가입되지 않은 유저가 해당 채널로 메시지 전송 시도 시 ERR_CANNOTSENDTOCHAN (404) 반환
#### 채널 운영자(Operator) 전용 명령어 테스트 (KICK, INVITE, TOPIC)
- 정상 운영자 케이스
	- /kick #ft_irc bad_user, /invite guest #ft_irc, /topic #ft_irc NewTopic 실행 시 정상 반영
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
- /mode #ft_irc +i / -i (초대 전용 모드)
	- 정상 케이스: 운영자가 설정 후 초대받은 유저만 채널 입장 허용
	- 예외 및 오류 처리: +i 상태인 채널에 초대(INVITE)받지 않은 일반 유저가 진입 시도 시 ERR_INVITEONLYCHAN (473) 반환
- /mode #ft_irc +t / -t (주제 변경 제한 모드)
	- 정상 케이스: +t 설정 시 운영자만 채널 주제 수정 가능
	- 예외 및 오류 처리: +t 상태에서 권한이 없는 일반 유저가 /topic으로 주제 변경 시도 시 ERR_CHANOPRIVSNEEDED (482) 반환
- /mode #ft_irc +k secret123 / -k (채널 비밀번호 모드)
	- 정상 케이스: 올바른 비밀번호를 입력한 유저만 입장 허용
	- 예외 및 오류 처리: 비밀번호가 설정된 채널에 틀린 비밀번호로 입장 시도 시 ERR_BADCHANNELKEY (475) 반환
- /mode #ft_irc +l 5 / -l (인원 제한 모드)
	- 정상 케이스: 채널 내 동시 접속자 수를 최대 5명으로 제한
	- 예외 및 오류 처리: 정원이 가득 찬(+l 정원 초과) 채널에 추가로 입장 시도 시 ERR_CHANNELISFULL (471) 반환
- /mode #ft_irc +o user / -o user (운영자 권한 부여/회수)
	- 정상 케이스: 타 유저에게 운영자 등급을 양도하거나 회수 가능
	- 예외 및 오류 처리: 채널 내에 존재하지 않는 유저에게 권한을 부여하려 할 경우 ERR_USERNOTINCHANNEL (441) 반환
### nc
수동 명령어 입력 및 로우 데이터(Raw Data) 검증 클라이언트
#### 기본 접속 및 인증 테스트
nc -C 127.0.0.1 6667
후 PASS password, NICK user, USER user 0 * :user 수동 전송
- 정상 케이스
	- 비밀번호 일치 시 핸드셰이크 완료 및 001 Welcome 메시지 수신 성공
- 예외 및 오류 처리
	- 잘못된 비밀번호 입력 후 NICK/USER 전송 시 서버에서 연결을 승인하지 않고 ERR_PASSWDMISMATCH (464) 반환 후 접속 차단
- 예외 및 오류 처리
	- 이미 서버에 존재하는 닉네임 전송 시도 시 ERR_NICKNAMEINUSE (433) 반환 및 새로운 닉네임 재입력 요구
#### 채널 입장 및 메시지 전송 테스트
JOIN #ft_irc 및 PRIVMSG #ft_irc :hello 수동 전송
- 정상 케이스
	- 채널 생성 또는 입장 성공 후 해당 채널로의 PRIVMSG 데이터 브로드캐스팅 수신 정상 작동
- 예외 및 오류 처리
	- 유효하지 않은 채널 이름 형식(예: # 없이 JOIN ft_irc 입력)으로 접근 시 ERR_NOSUCHCHANNEL (403) 반환
- 예외 및 오류 처리
	- 채널에 가입되지 않은 유저가 해당 채널로 PRIVMSG 전송 시도 시 ERR_CANNOTSENDTOCHAN (404) 반환
#### 채널 운영자(Operator) 전용 명령어 테스트 (KICK, INVITE, TOPIC)
- 정상 운영자 케이스
	- KICK #ft_irc bad_user, INVITE guest #ft_irc, TOPIC #ft_irc NewTopic 실행 시 정상 반영 및 변경 사항 수신
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
- MODE #ft_irc +i / -i (초대 전용 모드)
	- 정상 케이스: 운영자가 설정 후 초대받은 유저만 채널 입장 허용
	- 예외 및 오류 처리: +i 상태인 채널에 INVITE 받지 않은 일반 유저가 JOIN 시도 시 ERR_INVITEONLYCHAN (473) 반환
- MODE #ft_irc +t / -t (주제 변경 제한 모드)
	- 정상 케이스: +t 설정 시 운영자만 채널 주제 수정 가능
	- 예외 및 오류 처리: +t 상태에서 권한이 없는 일반 유저가 TOPIC으로 주제 변경 시도 시 ERR_CHANOPRIVSNEEDED (482) 반환
- MODE #ft_irc +k secret123 / -k (채널 비밀번호 모드)
	- 정상 케이스: 올바른 비밀번호를 입력(JOIN #ft_irc secret123)한 유저만 입장 허용
	- 예외 및 오류 처리: 비밀번호가 설정된 채널에 틀린 비밀번호 또는 비밀번호 없이 입장 시도 시 ERR_BADCHANNELKEY (475) 반환
- MODE #ft_irc +l 5 / -l (인원 제한 모드)
	- 정상 케이스: 채널 내 동시 접속자 수를 최대 5명으로 제한
	- 예외 및 오류 처리: 정원이 가득 찬(+l 정원 초과) 채널에 추가로 JOIN 시도 시 ERR_CHANNELISFULL (471) 반환
- MODE #ft_irc +o user / -o user (운영자 권한 부여/회수)
	- 정상 케이스: 타 유저에게 운영자 등급을 양도하거나 회수 가능
	- 예외 및 오류 처리: 채널 내에 존재하지 않는 유저에게 권한을 부여하려 할 경우 ERR_USERNOTINCHANNEL (441) 반환




# Resources

## RFC 1459 - Internet Relay Chat Protocol
https://datatracker.ietf.org/doc/html/rfc1459

- IRC 프로토콜의 표준 아키텍처와 기본 메시지 포맷, 클라이언트-서버 간의 통신 규약을 이해하기 위해 참조
- 명령어(KICK, INVITE, TOPIC, MODE 등)의 기본적인 동작 원리와 에러 커맨드(Numeric Replies)의 표준 규격을 학습


## Modern IRC Client Protocol
https://modern.ircdocs.horse/
- RFC 1459 표준 이후 현대적인 IRC 클라이언트들이 사용하는 프로토콜 명세와 차이점을 파악하기 위해 참조


## Beej's Guide to Network Programming
https://beej.us/guide/bgnet/html/split/
- C/C++ 기반의 소켓 프로토콜과 네트워크 프로그래밍의 기초


## Use of AI
### 번역
본 프로젝트의 최종 README.md 문서를 한국어에서 영어로 번역하고 문맥을 매끄럽게 다듬는 과정에 생성형 AI를 활용