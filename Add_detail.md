/kick을 하는 경우 window가 안닫히는 것에 관해
https://irssi.org/documentation/settings/
autoclose_query 0
autoclose_query 초 동안 활동이 없으면 쿼리 창이 자동으로 닫힙니다. autoclose_query 값을 0으로 설정하면 사용자가 직접 닫을 때까지 창이 열린 상태로 유지됩니다.

autoclose_windows ON
아무도 없는 창은 자동으로 닫힙니다. 이렇게 하면 창 목록이 깔끔하게 유지되지만, 사용자가 로그아웃한 후 나중에 개인 메시지를 보내면 쿼리 창의 순서가 바뀔 수 있습니다.

->kick은  사용자가 직접 닫는게 아니므로 /window 1로 수동으로 이동해야 함

/mode +o  /nick
브로드캐스트 루프 추가