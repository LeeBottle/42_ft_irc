# ft_irc 코드 검토 및 수정 사항

검토 대상: `/home/sanghul2/p/irc`

검토 기준:

- `ftirc.pdf` mandatory 요구사항
- `c++ -Wall -Wextra -Werror -std=c++98`
- 실제 IRC 클라이언트 접속 가능성

## 현재 상태 요약

현재 코드는 ft_irc 서버라기보다는 epoll 기반 TCP 브로드캐스트 예제에 가깝다. `PASS` 일부 처리와 클라이언트 간 메시지 전달은 있지만, IRC 프로토콜의 필수 흐름인 `PASS`, `NICK`, `USER`, `JOIN`, `PRIVMSG`, 채널/오퍼레이터/모드 처리가 거의 구현되어 있지 않다.

또한 기본 `make re`는 성공하지만, 과제 필수 플래그가 실제 컴파일에 적용되지 않는다. 필수 기준인 `-Wall -Wextra -Werror -std=c++98`로 확인하면 컴파일이 실패한다.

## 우선순위 1: 바로 고쳐야 하는 빌드 문제

### 1. Makefile이 과제 필수 플래그를 적용하지 않음

위치: `provided_code_cpp/Makefile`

문제:

- `CFLAGS = -I. -g -std=c++98`만 있고 `-Wall -Wextra -Werror`가 없다.
- `.cpp.o` 명시 규칙이 없어 기본 암시 규칙으로 컴파일된다.
- 실제 `make re` 출력은 `g++ -c -o main.o main.cpp` 형태라 `CFLAGS`가 적용되지 않는다.
- 루트 디렉터리 `/home/sanghul2/p/irc`에는 Makefile이 없다. 평가 저장소 루트에서 `make`를 실행하면 실패할 가능성이 높다.

수정 방향:

- 루트에 Makefile을 두거나 소스 구조를 루트 기준으로 정리한다.
- `CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I...`를 사용한다.
- 명시적인 컴파일 규칙을 추가한다.

예시:

```make
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I.

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

### 2. C++98 기준으로 컴파일되지 않음

위치:

- `provided_code_cpp/client_read.cpp:60`
- `provided_code_cpp/init_epoll.cpp:14`
- `provided_code_cpp/bircd.hpp:74`

문제:

- `std::to_string`은 C++11 기능이다.
- `nullptr`도 C++11 기능이다.
- `env` 생성자 초기화 순서가 멤버 선언 순서와 달라 `-Werror`에서 실패한다.

확인 명령:

```sh
c++ -Wall -Wextra -Werror -std=c++98 -I. -c client_read.cpp
c++ -Wall -Wextra -Werror -std=c++98 -I. -c init_epoll.cpp
```

수정 방향:

- `std::to_string` 대신 `std::stringstream` 사용
- `nullptr` 대신 `NULL` 사용
- `bircd.hpp`의 생성자 초기화 리스트를 멤버 선언 순서와 맞춘다.

현재 선언 순서:

```cpp
std::vector<fd> fds;
int port;
int max;
int r;
int epoll_fd;
```

현재 초기화:

```cpp
env() : port(0), epoll_fd(-1), max(0), r(0) {}
```

수정 예:

```cpp
env() : port(0), max(0), r(0), epoll_fd(-1), password("") {}
```

### 3. `init_fd.cpp`는 소스에 남아 있지만 현재 구조와 맞지 않음

위치: `provided_code_cpp/init_fd.cpp`

문제:

- `fd_read`, `fd_write`, `maxfd`가 선언되어 있지 않다.
- Makefile의 `SRCS`에는 빠져 있어서 현재 빌드에는 포함되지 않는다.
- select 기반 이전 코드의 잔재로 보인다.

수정 방향:

- epoll만 사용할 계획이면 `init_fd.cpp`, `check_fd.cpp`, `do_select`라는 이름 등 select 잔재를 정리한다.
- 남길 계획이면 `env`에 필요한 멤버를 명확히 추가하고 실제로 사용해야 한다.

## 우선순위 2: 과제 요구사항 미충족

### 4. FD가 non-blocking으로 설정되지 않음

위치:

- `provided_code_cpp/srv_create.cpp:15`
- `provided_code_cpp/srv_accept.cpp:10`

문제:

- 과제는 모든 I/O가 non-blocking이어야 한다.
- 현재 서버 소켓과 클라이언트 소켓 모두 `fcntl(fd, F_SETFL, O_NONBLOCK)` 설정이 없다.
- epoll을 사용하더라도 FD 자체가 blocking이면 요구사항을 만족하지 못한다.

수정 방향:

- 서버 소켓 생성 직후 non-blocking 설정
- accept된 클라이언트 소켓도 즉시 non-blocking 설정
- `accept`, `read`, `write`에서 `EAGAIN`, `EWOULDBLOCK`, `EINTR` 처리

### 5. write/send를 이벤트 큐 없이 직접 호출하는 부분이 있음

위치: `provided_code_cpp/client_read.cpp:48`

문제:

- 비밀번호 오류 시 `send()`를 즉시 호출한다.
- 과제는 read/write/send/recv 모두 poll 또는 equivalent 이벤트 처리 흐름 안에서 관리해야 한다.
- 현재처럼 읽기 처리 중 바로 `send()`를 호출하면 평가에서 감점 또는 0점 위험이 있다.

수정 방향:

- 에러 응답도 `buf_write`에 넣고 `EPOLLOUT`을 등록한다.
- 전송 완료 후 연결을 닫아야 하는 상태 플래그를 클라이언트에 저장한다.

### 6. partial command 처리가 안 됨

위치: `provided_code_cpp/client_read.cpp:25`

문제:

- 읽은 버퍼 `buf`만 바로 파싱한다.
- `fd::buf_read` 멤버는 있지만 사용하지 않는다.
- 과제 예시처럼 `com`, `man`, `d\n`처럼 나뉘어 들어오는 명령을 조립하지 못한다.

수정 방향:

- `fds[cs].buf_read += received_chunk`
- `\r\n` 또는 `\n`이 나올 때까지 누적
- 완성된 한 줄씩 parser에 넘기고, 남은 조각은 버퍼에 보존

### 7. 인증 상태가 저장되지 않음

위치: `provided_code_cpp/client_read.cpp:25-54`

문제:

- `PASS` 문자열이 포함되어 있으면 비밀번호만 검사한다.
- 인증 성공 여부를 클라이언트별로 저장하지 않는다.
- `PASS` 없이도 메시지가 다른 클라이언트에게 브로드캐스트된다.
- `PASS`가 명령어가 아니라 메시지 중간에 포함되어도 인증 로직이 실행될 수 있다.

수정 방향:

- `Client` 상태 객체를 만들고 `pass_ok`, `nick_set`, `user_set`, `registered`를 저장한다.
- IRC 명령어 단위로 파싱해서 첫 토큰이 정확히 `PASS`일 때만 처리한다.
- 등록 완료 전에는 제한된 명령만 허용한다.

### 8. IRC 필수 명령어가 거의 없음

필수 구현 필요:

- `PASS`
- `NICK`
- `USER`
- `JOIN`
- `PRIVMSG`
- `KICK`
- `INVITE`
- `TOPIC`
- `MODE i/t/k/o/l`

현재 상태:

- `PASS` 일부 처리만 있음
- 닉네임, 유저네임, 채널, 개인 메시지, 채널 메시지, 오퍼레이터, 모드가 구현되어 있지 않음
- 현재 브로드캐스트는 IRC 메시지 형식이 아니라 `[Client fd]: ...` 형태다.

수정 방향:

- `Client` 클래스와 `Channel` 클래스를 먼저 만든다.
- 명령어 파서를 만든 뒤 각 command handler로 분리한다.
- 응답은 IRC numeric reply 형식을 맞춘다.

### 9. 채널 자료구조가 없음

문제:

- 채널 멤버 목록이 없다.
- 채널 오퍼레이터 목록이 없다.
- invite-only, topic 제한, key, user limit 같은 mode 상태가 없다.

수정 방향:

`Channel`에 최소한 아래 상태가 필요하다.

```cpp
std::string name;
std::string topic;
std::string key;
bool inviteOnly;
bool topicOperatorOnly;
int userLimit;
std::set<int> members;
std::set<int> operators;
std::set<int> invited;
```

C++98에서는 `std::unordered_map`을 쓸 수 없으므로 `std::map<std::string, Channel>` 같은 구조를 사용한다.

### 10. 클라이언트 상태 자료구조가 부족함

현재 `fd`에는 FD 종류, 함수 포인터, read/write 버퍼만 있다.

ft_irc에는 최소한 아래 정보가 필요하다.

```cpp
int fd;
std::string nickname;
std::string username;
std::string hostname;
bool passOk;
bool registered;
bool closeAfterWrite;
std::string readBuffer;
std::string writeBuffer;
```

수정 방향:

- 현재 `fd` 클래스를 네트워크 이벤트용으로만 둘지, `Client`와 합칠지 결정한다.
- 권장: `Client` 클래스를 따로 만들고 `std::map<int, Client>`로 관리한다.

## 우선순위 3: 안정성 문제

### 11. 오류에서 `exit(1)`을 너무 많이 사용함

위치:

- `provided_code_cpp/srv_accept.cpp:13-14`
- `provided_code_cpp/do_select.cpp:11-12`
- `provided_code_cpp/srv_create.cpp`
- `provided_code_cpp/init_env.cpp`

문제:

- 과제는 서버가 쉽게 죽으면 안 된다.
- 특히 `accept()` 실패 하나로 전체 서버가 종료되면 안 된다.

수정 방향:

- 초기화 단계의 치명적 오류는 종료 가능
- 런타임 클라이언트 I/O 오류는 해당 클라이언트만 정리
- `EINTR`, `EAGAIN`, `EWOULDBLOCK`은 정상 흐름으로 처리

### 12. `write()` 부분 전송과 에러 처리가 부족함

위치: `provided_code_cpp/client_write.cpp:7-18`

문제:

- `write()`가 0을 반환하는 경우 처리가 없다.
- non-blocking 전환 후 `EAGAIN`, `EWOULDBLOCK`을 에러로 닫으면 안 된다.
- `write()`의 반환 타입은 `ssize_t`인데 현재 `int`로 받는다.

수정 방향:

- `ssize_t sent` 사용
- `sent > 0`, `sent == 0`, `sent < 0` 케이스 분리
- `EAGAIN/EWOULDBLOCK/EINTR`은 연결 유지

### 13. FD 인덱스 접근 범위 검사가 없음

위치:

- `provided_code_cpp/srv_create.cpp:36`
- `provided_code_cpp/srv_accept.cpp:20-23`
- `provided_code_cpp/check_epoll.cpp`

문제:

- FD 값이 `fds.size()` 이상이면 out-of-bounds 접근이 된다.
- `getrlimit(RLIMIT_NOFILE)`로 resize하긴 하지만, 방어 코드는 없다.

수정 방향:

- FD가 벡터 범위 안인지 확인
- 더 안전하게는 `std::map<int, Client>`로 FD 기반 객체를 관리

### 14. 포트 검증이 부족함

위치: `provided_code_cpp/get_opt.cpp:10-16`

문제:

- 숫자 여부만 확인한다.
- `0`, 음수, `65535` 초과 같은 포트가 걸러지지 않는다.

수정 방향:

- `1 <= port <= 65535` 검증
- 실사용 권장 범위는 일반 유저 실행을 고려해 `1024 <= port <= 65535`

## 우선순위 4: 제출/평가 준비 문제

### 15. README.md가 과제 요구사항을 만족하지 않음

위치: `README.md`

문제:

- 첫 줄이 과제 지정 문구가 아니다.
- 영어 Description, Instructions, Resources 섹션이 없다.
- AI 사용 여부/사용 범위 설명이 없다.

수정 방향:

README 첫 줄은 아래 형식이어야 한다.

```md
*This project has been created as part of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].*
```

그리고 최소 섹션:

- `Description`
- `Instructions`
- `Resources`

### 16. 빌드 산출물이 git status에 남아 있음

현재 `git status --short`에서 아래 파일들이 untracked로 보인다.

- `provided_code_cpp/*.o`
- `provided_code_cpp/ircserv`

수정 방향:

- `.gitignore` 추가
- 기존 산출물 제거

예시:

```gitignore
*.o
ircserv
```

## 권장 구현 순서

1. 저장소 구조 정리: 루트 Makefile, `.gitignore`, 불필요한 `init_fd.cpp`/select 잔재 정리
2. 필수 플래그로 컴파일되게 수정: C++98 위반 제거, `-Wall -Wextra -Werror` 적용
3. FD non-blocking 설정과 epoll 이벤트 처리 안정화
4. `Client`, `Channel`, `Server` 구조 설계
5. 수신 버퍼 누적 및 IRC line parser 구현
6. 등록 흐름 구현: `PASS`, `NICK`, `USER`
7. 기본 IRC 메시지 구현: `JOIN`, `PRIVMSG`
8. 채널 오퍼레이터 명령 구현: `KICK`, `INVITE`, `TOPIC`, `MODE`
9. irssi 같은 기준 클라이언트로 실제 접속 테스트
10. `nc -C` partial command 테스트
11. README.md 완성

## 최소 테스트 목록

### 빌드

```sh
make fclean
make
make re
```

필수 확인:

```sh
c++ -Wall -Wextra -Werror -std=c++98 ...
```

### 실행

```sh
./ircserv 6667 pass
```

### nc partial command

```sh
nc -C 127.0.0.1 6667
```

테스트 내용:

- `PASS pass`를 여러 조각으로 나눠 보내도 처리되는지
- `NICK`, `USER` 등록 전후 응답이 맞는지
- 잘못된 비밀번호에서 에러 응답 후 연결 종료되는지

### IRC 클라이언트

예시 기준 클라이언트: `irssi`

```sh
/connect 127.0.0.1 6667 pass
/nick user1
/join #test
/msg #test hello
```

## 결론

현재 코드는 네트워크 이벤트 루프의 출발점으로는 사용할 수 있지만, ft_irc mandatory 제출물로는 아직 부족하다. 가장 먼저 빌드 기준을 과제 플래그에 맞추고, non-blocking + epoll 기반의 안정적인 입출력 골격을 만든 뒤, IRC 명령어와 채널 상태를 새로 설계하는 방향이 필요하다.
