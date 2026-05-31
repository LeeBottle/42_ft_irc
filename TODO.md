# ft_irc TODO

현재 브랜치: `sanghul2`

현재 상태:

- `make`와 `make re`는 `-Wall -Wextra -Werror -std=c++98`로 통과한다.
- `PASS`, `NICK`, `USER`, `PRIVMSG` 일부 처리가 있다.
- partial command 누적 처리는 동작 확인됨.
- 아직 mandatory 핵심인 `JOIN`, 채널, operator command, mode 처리는 없다.

## 0. 작업 전 정리

- [x] `.gitignore` 추가
  - [x] `*.o`
  - [x] `*.tmp`
  - [x] `ircserv`
- [x] 현재 untracked 산출물 정리
  - [x] `ircserv`
  - [x] `*.o`
- [ ] 예전 코드 폴더 `provided_code_cpp/`를 유지할지 삭제할지 결정
- [ ] 문서 파일을 repo에 남길지 결정
  - [ ] `ft_irc_review.md`
  - [ ] `rfc_implementation_notes.md`
  - [ ] `test_result_2026-05-29.md`

## 1. 네트워크/이벤트 루프 안정화

- [x] 서버 소켓 non-blocking 설정
  - 대상: `srv_create.cpp`
  - `fcntl(server_fd, F_SETFL, O_NONBLOCK)`
- [x] accept된 클라이언트 소켓 non-blocking 설정
  - 대상: `srv_accept.cpp`
  - `fcntl(client_fd, F_SETFL, O_NONBLOCK)`
- [x] `accept()` 에러 처리 개선
  - [x] `EAGAIN`, `EWOULDBLOCK`, `EINTR`은 서버 종료하지 않기
  - [x] 실제 오류만 로그 처리
- [x] `read()` 에러 처리 개선
  - [x] `EAGAIN`, `EWOULDBLOCK`, `EINTR` 처리
  - [x] `r == 0`일 때 정상 disconnect 처리
- [x] `write()` 에러 처리 개선
  - [x] 반환 타입 `int`에서 `ssize_t`로 변경
  - [x] partial write 유지
  - [x] `EAGAIN`, `EWOULDBLOCK`, `EINTR` 처리
- [x] `send()` 직접 호출 제거
  - 모든 응답은 `buf_write`에 넣고 `EPOLLOUT`에서 전송

## 2. 클라이언트 상태 구조 정리

- [ ] `fd` 클래스에 등록 상태 추가
  - [ ] `bool pass_ok`
  - [ ] `bool registered`
  - [x] `bool close_after_write`
- [x] `clean_fd()`에서 새 상태 초기화
- [ ] `type == FD_CLIENT`를 PASS 성공 의미로 쓰지 않도록 분리
  - `type`은 FD 종류만 표현
  - 인증/등록은 별도 bool로 관리
- [ ] welcome `001` 중복 전송 방지
  - `registered == false`에서 `PASS + NICK + USER`가 모두 완료될 때 한 번만 전송
- [ ] 연결 종료 예약 함수 만들기
  - 예: `mark_close_after_write(int fd)`

## 3. IRC parser 개선

- [ ] command line parser 함수 분리
  - [ ] command
  - [ ] params
  - [ ] trailing parameter
- [ ] command 대소문자 무시 처리
  - 예: `pass`, `Pass`, `PASS` 모두 처리
- [ ] 빈 line 무시
- [ ] 한 line 길이 제한 처리
  - RFC 기준 CRLF 포함 512 bytes
- [ ] 등록 전 허용 명령 제한
  - [ ] `CAP`
  - [ ] `PASS`
  - [ ] `NICK`
  - [ ] `USER`
  - [ ] `PING`
- [ ] 알 수 없는 명령 `421 ERR_UNKNOWNCOMMAND` 응답

## 4. 등록 명령 정리

### PASS

- [ ] 인자 없으면 `461 ERR_NEEDMOREPARAMS`
- [ ] 이미 등록된 뒤 다시 오면 `462 ERR_ALREADYREGISTRED`
- [ ] 비밀번호 틀리면 `464 ERR_PASSWDMISMATCH`
- [ ] 틀린 비밀번호 응답 전송 후 close
- [ ] 현재 일반 문자열 응답 `Password authorized` 제거 또는 numeric으로 대체

### NICK

- [ ] 인자 없으면 `431 ERR_NONICKNAMEGIVEN`
- [ ] 닉네임 형식 검증
- [ ] 잘못된 닉이면 `432 ERR_ERRONEUSNICKNAME`
- [ ] 중복 닉이면 `433 ERR_NICKNAMEINUSE`
- [ ] 닉 변경 시 관련 채널에 `NICK` 메시지 전파

### USER

- [ ] 인자 부족하면 `461 ERR_NEEDMOREPARAMS`
- [ ] 이미 등록된 뒤 다시 오면 `462 ERR_ALREADYREGISTRED`
- [ ] username과 realname 저장
- [ ] `mode`, `unused`는 파싱만 하고 무시

### 등록 완료 응답

- [ ] `001 RPL_WELCOME`
- [ ] `002 RPL_YOURHOST`
- [ ] `003 RPL_CREATED`
- [ ] `004 RPL_MYINFO`

## 5. PING/CAP 호환

- [ ] `PING :token` 수신 시 `PONG :token`
- [ ] `CAP LS` 처리
  - 최소 응답: `CAP * LS :`
  - 또는 조용히 무시할 경우 irssi 동작 확인

## 6. Channel 구조 추가

- [ ] `Channel` 클래스 또는 struct 추가
- [ ] 최소 필드
  - [ ] `std::string name`
  - [ ] `std::string topic`
  - [ ] `std::string key`
  - [ ] `bool invite_only`
  - [ ] `bool topic_op_only`
  - [ ] `int user_limit`
  - [ ] `std::set<int> members`
  - [ ] `std::set<int> operators`
  - [ ] `std::set<int> invited`
- [ ] `env`에 channel map 추가
  - `std::map<std::string, Channel>`
- [ ] C++98 기준 include 추가
  - [ ] `<map>`
  - [ ] `<set>`

## 7. JOIN 구현

- [ ] `JOIN <channel> [key]`
- [ ] channel 이름 검증
- [ ] 없는 채널이면 생성
- [ ] 첫 입장자를 operator로 설정
- [ ] 이미 들어간 채널이면 중복 처리 방지
- [ ] `+i` invite-only 검사
- [ ] `+k` key 검사
- [ ] `+l` user limit 검사
- [ ] 성공 시 채널 멤버에게 JOIN 전파
- [ ] topic 응답
  - [ ] topic 없음: `331 RPL_NOTOPIC`
  - [ ] topic 있음: `332 RPL_TOPIC`
- [ ] names 응답
  - [ ] `353 RPL_NAMREPLY`
  - [ ] `366 RPL_ENDOFNAMES`

## 8. PRIVMSG 정리

- [ ] 현재 전체 FD 브로드캐스트 제거
- [ ] `PRIVMSG <nick> :message` 구현
- [ ] `PRIVMSG #channel :message` 구현
- [ ] channel 메시지는 같은 채널의 다른 멤버에게만 전파
- [ ] 인자 없음: `411 ERR_NORECIPIENT`
- [ ] text 없음: `412 ERR_NOTEXTTOSEND`
- [ ] 대상 nick 없음: `401 ERR_NOSUCHNICK`
- [ ] 대상 channel 없음: `403 ERR_NOSUCHCHANNEL`
- [ ] 전달 형식 RFC에 맞추기
  - `:nick!user@host PRIVMSG target :message`

## 9. Operator 명령 구현

### KICK

- [ ] `KICK <channel> <nick> [:reason]`
- [ ] operator 권한 검사
- [ ] 대상이 채널에 있는지 검사
- [ ] 성공 시 채널 멤버와 대상에게 KICK 전파
- [ ] 대상 제거

### INVITE

- [ ] `INVITE <nick> <channel>`
- [ ] 대상 nick 존재 검사
- [ ] 대상이 이미 채널에 있는지 검사
- [ ] invite-only 채널이면 operator 권한 검사
- [ ] invited 목록에 추가
- [ ] `341 RPL_INVITING`
- [ ] 대상에게 INVITE 메시지 전송

### TOPIC

- [ ] `TOPIC <channel>`
- [ ] `TOPIC <channel> :new topic`
- [ ] topic 조회
- [ ] topic 변경
- [ ] `+t`일 때 operator 권한 검사
- [ ] 변경 성공 시 채널에 TOPIC 전파

### MODE

- [ ] `MODE <channel>` 조회
- [ ] `+i`, `-i`
- [ ] `+t`, `-t`
- [ ] `+k <key>`, `-k`
- [ ] `+o <nick>`, `-o <nick>`
- [ ] `+l <limit>`, `-l`
- [ ] operator 권한 검사
- [ ] mode 변경 성공 시 채널에 MODE 전파

## 10. README 정리

- [ ] 첫 줄 subject 요구 형식으로 수정

```md
*This project has been created as part of the 42 curriculum by <login>.*
```

- [ ] 영어로 작성
- [ ] `Description` 섹션
- [ ] `Instructions` 섹션
- [ ] `Resources` 섹션
- [ ] AI 사용 여부와 사용 범위 설명
- [ ] 기준 IRC 클라이언트 명시

## 11. 테스트 체크리스트

### 빌드

- [ ] `make fclean`
- [ ] `make`
- [ ] `make re`
- [ ] 불필요한 relink 없는지 확인

### nc

- [ ] 정상 등록
  - `PASS`, `NICK`, `USER`
- [ ] 잘못된 PASS
- [ ] partial command
  - `PA` + `SS pass\r\n`
- [ ] 한 recv에 여러 command
- [ ] 연결 종료 처리

### irssi

- [ ] `/connect 127.0.0.1 <port> <password>`
- [ ] 자동 `CAP/PASS/NICK/USER` 처리
- [ ] `/join #test`
- [ ] 두 클라이언트 간 `PRIVMSG #test`
- [ ] `/topic`
- [ ] `/kick`
- [ ] `/invite`
- [ ] `/mode #test +i`
- [ ] `/mode #test +k key`
- [ ] `/mode #test +l 2`
- [ ] `/mode #test +o nick`

## 12. 다음 작업 추천 순서

1. `.gitignore`와 산출물 정리
2. FD non-blocking 적용
3. `close_after_write` 추가해서 잘못된 PASS 응답 전송 보장
4. `pass_ok`, `registered` 상태 분리
5. parser 구조 개선
6. `JOIN`과 `Channel` 구현
7. channel `PRIVMSG`
8. operator command 구현
