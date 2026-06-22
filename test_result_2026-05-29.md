# ft_irc pull 이후 테스트 결과

일시: 2026-05-29

브랜치: `c++styling`

pull 결과:

```text
bcde97b..f55e93f fast-forward
```

갱신 파일:

- `README.md`
- `bircd.hpp`
- `clean_fd.cpp`
- `client_read.cpp`
- `client_read_broadcast.cpp`
- `client_read_handle.cpp`

## 1. 기본 make 테스트

명령:

```sh
make fclean
make
```

결과: 성공

주의:

- 현재 Makefile은 실제 컴파일에서 `CFLAGS`가 적용되지 않는다.
- 빌드 출력이 `g++ -c -o main.o main.cpp` 형태로 나오며, `-Wall -Wextra -Werror -std=c++98`가 붙지 않는다.

## 2. 과제 필수 플래그 테스트

명령:

```sh
c++ -Wall -Wextra -Werror -std=c++98 -I. -c main.cpp init_env.cpp clean_fd.cpp get_opt.cpp main_loop.cpp do_epoll.cpp srv_create.cpp srv_accept.cpp client_read.cpp client_read_handle.cpp client_read_broadcast.cpp client_write.cpp init_epoll.cpp check_epoll.cpp
```

결과: 실패

오류:

- `bircd.hpp:76`: 생성자 초기화 순서가 멤버 선언 순서와 다름
- `client_read_broadcast.cpp:17`: `std::to_string`은 C++98에 없음

수정 필요:

```cpp
env() : port(0), max(0), r(0), epoll_fd(-1), password("") {}
```

그리고 `std::to_string(sender_cs)`는 `std::stringstream`으로 바꿔야 한다.

## 3. 로컬 실행 테스트

명령:

```sh
./ircserv 6669 pass
```

일반 sandbox에서는 socket 생성이 `Operation not permitted`로 실패했지만, 권한 허용 후 실행 성공.

## 4. 기본 등록 테스트

명령:

```sh
printf 'PASS pass\r\nNICK a\r\nUSER a 0 * :A User\r\n' | nc -w 1 127.0.0.1 6669
```

응답:

```text
Password authorized
:ircserv 001 a :Welcome to the Internet Relay Network a
```

결과: 부분 성공

의미:

- `PASS`, `NICK`, `USER`가 한 TCP chunk로 들어와도 line 단위로 처리된다.
- 등록 완료 후 `001` welcome 응답을 보낸다.

남은 문제:

- `Password authorized`는 RFC numeric reply가 아니다.
- `001`이 중복 전송될 가능성이 있다. `NICK`과 `USER` 각각에서 조건이 맞으면 welcome을 보낸다.
- 등록 완료 상태 플래그가 따로 없어 이미 welcome을 보냈는지 구분하지 않는다.

## 5. 잘못된 PASS 테스트

명령:

```sh
printf 'PASS wrong\r\n' | nc -w 1 127.0.0.1 6669
```

응답:

```text
```

결과: 실패

서버 로그에서는 `Password incorrect.`를 write buffer에 넣지만 바로 `epoll_del`, `close`, `clean_fd`를 실행한다. 그래서 클라이언트가 에러 응답을 받지 못한다.

수정 필요:

- 잘못된 비밀번호 응답을 `buf_write`에 넣는다.
- `closeAfterWrite` 같은 플래그를 세운다.
- `EPOLLOUT`에서 전송 완료 후 연결을 닫는다.

## 6. partial command 테스트

명령:

```sh
{ printf 'PA'; sleep 0.1; printf 'SS pass\r\nNI'; sleep 0.1; printf 'CK part\r\nUSER part 0 * :Partial User\r\n'; } | nc -w 1 127.0.0.1 6669
```

응답:

```text
Password authorized
:ircserv 001 part :Welcome to the Internet Relay Network part
```

결과: 성공

의미:

- `buf_read` 누적 후 `\n` 기준으로 line을 꺼내는 구조가 동작한다.
- subject의 partial packet 요구사항 방향은 개선됐다.

## 7. 아직 큰 범위에서 미구현인 부분

현재 pull 이후에도 mandatory 기준으로 남은 큰 항목:

- FD non-blocking 설정 없음
- `JOIN` 미구현
- 채널 자료구조 없음
- 채널 메시지 브로드캐스트가 IRC `PRIVMSG #channel :message` 형식이 아님
- `KICK` 미구현
- `INVITE` 미구현
- `TOPIC` 미구현
- `MODE i/t/k/o/l` 미구현
- IRC numeric error 대부분 미구현
- `send()`/`write()`의 non-blocking 에러 처리 부족
- `README.md` 첫 줄이 subject 필수 문구 형식이 아님

## 결론

이번 업데이트로 line buffer 기반 partial command 처리와 `PASS/NICK/USER/PRIVMSG` 분리가 추가됐다. 기본 등록 흐름은 `nc`에서 동작한다.

하지만 과제 필수 플래그 컴파일은 아직 실패하며, mandatory IRC 서버로 보기에는 `JOIN`, channel, operator command, mode 처리가 빠져 있다. 다음 우선순위는 Makefile/컴파일 기준 수정, C++98 오류 제거, non-blocking FD 설정, 잘못된 PASS 응답 후 close 처리다.
