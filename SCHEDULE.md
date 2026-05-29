# ft_irc 추천 일정

전제:

- 목표는 mandatory 통과 수준이다.
- 기준 클라이언트는 `irssi`로 둔다.
- 현재 브랜치 `sanghul2`의 상태에서 이어서 진행한다.
- 현재 이미 된 것:
  - `make`, `make re`가 `-Wall -Wextra -Werror -std=c++98`로 통과
  - epoll 기반 이벤트 루프 존재
  - read buffer 누적 후 line 단위 처리 존재
  - `PASS`, `NICK`, `USER`, `PRIVMSG` 일부 구현
  - partial command 기본 테스트 통과

## 전체 전략

하루 만에 모든 기능을 “작성”하는 것은 가능할 수 있지만, 평가에서 버티는 수준까지는 테스트와 설명 준비가 필요하다. 따라서 추천 일정은 4일 집중 + 1일 안정화다.

가장 중요한 원칙:

- 기능을 크게 만들지 말고 작은 단위로 끝낸다.
- 각 단계마다 `make re`를 통과시킨다.
- `nc`와 `irssi`로 바로 테스트한다.
- 새 기능을 만들 때마다 내가 설명할 수 있는 구조인지 확인한다.

## 빠른 일정: 2일 프로토타입

시간이 아주 부족할 때의 일정이다.

### 1일차

목표:

- 서버 안정성 골격 완성
- 등록 흐름 정리
- `JOIN`
- 채널 `PRIVMSG`

작업:

- [ ] non-blocking FD 설정
- [ ] `accept/read/write` 에러 처리
- [ ] `close_after_write` 추가
- [ ] `pass_ok`, `registered` 상태 분리
- [ ] `PASS/NICK/USER` numeric reply 정리
- [ ] `PING`, `CAP` 최소 처리
- [ ] `Channel` 구조 추가
- [ ] `JOIN #channel` 구현
- [ ] 채널 `PRIVMSG` 구현

검증:

- [ ] `make re`
- [ ] `nc`로 partial command
- [ ] `irssi` 접속
- [ ] 두 클라이언트가 같은 채널에서 대화

### 2일차

목표:

- channel operator 명령 구현
- README와 평가 대비 정리

작업:

- [ ] `KICK`
- [ ] `INVITE`
- [ ] `TOPIC`
- [ ] `MODE +i/-i`
- [ ] `MODE +t/-t`
- [ ] `MODE +k/-k`
- [ ] `MODE +o/-o`
- [ ] `MODE +l/-l`
- [ ] README subject 요구사항 반영
- [ ] irssi 통합 테스트

검증:

- [ ] 두 개 이상의 irssi 클라이언트 테스트
- [ ] operator와 일반 유저 권한 차이 테스트
- [ ] 잘못된 입력에서 서버가 죽지 않는지 확인

위험:

- 하루에 operator 명령까지 몰아치면 mode edge case가 불안정할 수 있다.
- 평가 전 설명 준비 시간이 부족하다.

## 추천 일정: 5일

가장 현실적인 일정이다. 에이전트가 구현을 많이 도와도, 사용자가 이해하고 테스트할 시간을 확보하는 방식이다.

## Day 1: 서버 안정성 정리

목표:

- 과제의 non-blocking 요구사항을 만족한다.
- 잘못된 연결/입력 때문에 서버가 쉽게 죽지 않게 만든다.

작업:

- [ ] `.gitignore` 적용 상태 확인
- [ ] 빌드 산출물 정리
- [ ] 서버 소켓 non-blocking 설정
- [ ] 클라이언트 소켓 non-blocking 설정
- [ ] `accept()` 에러 처리
  - [ ] `EAGAIN`
  - [ ] `EWOULDBLOCK`
  - [ ] `EINTR`
- [ ] `read()` 에러 처리
  - [ ] `r > 0`
  - [ ] `r == 0`
  - [ ] `r < 0`
- [ ] `write()` 에러 처리
  - [ ] `ssize_t` 사용
  - [ ] partial write 유지
  - [ ] `EAGAIN/EWOULDBLOCK/EINTR`
- [ ] 직접 `send()` 제거
- [ ] `close_after_write` 추가

완료 기준:

- [ ] `make re` 성공
- [ ] 정상 접속/종료해도 서버가 살아 있음
- [ ] 잘못된 PASS에서 에러 응답을 보낸 뒤 연결 종료
- [ ] 여러 클라이언트가 접속/종료해도 서버 유지

테스트:

```sh
make re
./ircserv 6667 pass
printf 'PASS wrong\r\n' | nc -w 1 127.0.0.1 6667
printf 'PASS pass\r\nNICK a\r\nUSER a 0 * :A User\r\n' | nc -w 1 127.0.0.1 6667
```

## Day 2: 등록 흐름과 parser 정리

목표:

- `PASS`, `NICK`, `USER`가 IRC 등록 흐름으로 동작한다.
- irssi가 접속할 때 불필요한 에러가 나지 않는다.

작업:

- [ ] `pass_ok`, `registered` 상태 분리
- [ ] welcome 중복 전송 방지
- [ ] command parser 분리
  - [ ] command
  - [ ] params
  - [ ] trailing
- [ ] command 대소문자 normalize
- [ ] `CAP` 최소 처리
- [ ] `PING/PONG` 처리
- [ ] `PASS` 에러 numeric 정리
- [ ] `NICK` 에러 numeric 정리
- [ ] `USER` 에러 numeric 정리
- [ ] 등록 완료 시 `001`, `002`, `003`, `004` 응답

완료 기준:

- [ ] irssi `/connect`가 에러 없이 붙음
- [ ] `NICK` 중복 시 적절한 에러
- [ ] 비밀번호 오류 시 응답 후 종료
- [ ] 등록 전 `PRIVMSG` 같은 명령은 거부

테스트:

```text
/connect 127.0.0.1 6667 pass
/nick testnick
```

## Day 3: Channel과 JOIN

목표:

- 채널에 들어갈 수 있다.
- 채널 멤버 목록과 operator 상태를 관리한다.
- 채널 메시지가 같은 채널 안에서만 전달된다.

작업:

- [ ] `Channel` 구조 추가
- [ ] `env`에 channel map 추가
- [ ] `JOIN <channel> [key]`
- [ ] 채널 생성
- [ ] 첫 입장자 operator 부여
- [ ] JOIN 메시지 전파
- [ ] topic 응답
  - [ ] `331`
  - [ ] `332`
- [ ] names 응답
  - [ ] `353`
  - [ ] `366`
- [ ] channel `PRIVMSG` 구현
- [ ] nick 대상 `PRIVMSG` 구현
- [ ] 전체 FD 브로드캐스트 제거

완료 기준:

- [ ] 두 irssi 클라이언트가 `#test`에 join 가능
- [ ] 한 명이 보낸 메시지가 같은 채널의 다른 사람에게만 전달
- [ ] 채널 밖 사용자는 메시지를 받지 않음
- [ ] 첫 입장자가 operator로 표시됨

테스트:

```text
/join #test
/msg #test hello
```

## Day 4: Operator 명령

목표:

- subject mandatory의 channel operator 명령을 구현한다.

작업:

- [ ] `KICK`
  - [ ] 권한 검사
  - [ ] 대상 채널 멤버 검사
  - [ ] KICK 전파
- [ ] `INVITE`
  - [ ] 대상 nick 검사
  - [ ] invited 목록 추가
  - [ ] `341`
- [ ] `TOPIC`
  - [ ] 조회
  - [ ] 변경
  - [ ] `+t` 권한 검사
- [ ] `MODE`
  - [ ] `+i/-i`
  - [ ] `+t/-t`
  - [ ] `+k/-k`
  - [ ] `+o/-o`
  - [ ] `+l/-l`

완료 기준:

- [ ] operator는 모든 명령 실행 가능
- [ ] 일반 유저는 operator 명령이 거부됨
- [ ] invite-only 채널에 초대받은 유저만 입장 가능
- [ ] key가 맞아야 입장 가능
- [ ] user limit이 동작
- [ ] operator 권한 부여/회수 동작

테스트:

```text
/mode #test +i
/invite user2 #test
/mode #test +k secret
/mode #test +l 2
/mode #test +o user2
/topic #test new topic
/kick user2 reason
```

## Day 5: 안정화와 평가 준비

목표:

- 평가에서 자주 찌르는 케이스를 막는다.
- README와 설명 준비를 끝낸다.

작업:

- [ ] README subject 요구사항 반영
- [ ] 기준 클라이언트 명시
- [ ] AI 사용 범위 작성
- [ ] 불필요한 파일 제거 또는 ignore
- [ ] `make re` 반복 확인
- [ ] partial command 테스트
- [ ] 동시 클라이언트 테스트
- [ ] 잘못된 명령/인자 테스트
- [ ] 서버가 죽지 않는지 확인
- [ ] 코드 설명 준비

완료 기준:

- [ ] repository root에서 `make` 가능
- [ ] `./ircserv <port> <password>` 실행 가능
- [ ] irssi 접속 가능
- [ ] mandatory 기능 전체 시연 가능
- [ ] 주요 구조를 말로 설명 가능

평가 때 설명해야 할 것:

- [ ] 왜 non-blocking이 필요한지
- [ ] epoll을 어디서 한 번 쓰는지
- [ ] read buffer로 partial packet을 어떻게 조립하는지
- [ ] client 등록 상태가 어디 저장되는지
- [ ] channel과 operator 상태가 어디 저장되는지
- [ ] mode가 JOIN/TOPIC/권한에 어떻게 영향을 주는지

## 하루 단위 작업 방식

각 기능마다 이 순서를 반복한다.

1. 작은 기능 하나 구현
2. `make re`
3. `nc` 최소 테스트
4. `irssi` 실제 테스트
5. 에러 케이스 하나 테스트
6. 커밋

권장 커밋 단위:

- `add non-blocking socket handling`
- `separate client registration state`
- `add IRC command parser`
- `implement JOIN and channel state`
- `implement channel PRIVMSG`
- `implement KICK and INVITE`
- `implement TOPIC and MODE`
- `update README`

## 우선순위 요약

가장 먼저 할 것:

1. non-blocking
2. close-after-write
3. registered 상태 분리
4. parser 정리
5. JOIN

나중에 해도 되는 것:

1. command 대소문자 normalize
2. 더 많은 numeric reply 문구 정교화
3. mode 조합 입력 고급 처리
4. README polish

하지 않아도 되는 것:

1. 서버 간 통신
2. 파일 전송
3. bot
4. WHO/WHOIS/LIST/MOTD 전체 구현
5. ban mask, voice, moderated mode
