mini_webserver_5
DATE:2026-04-30
1.커맨드 로직 함수로 변경
2.라우터 설정 파일 추가
- Router.cpp,Router.h
- 라우터 마임타입 설정 추가
--------------------------
mini_webserver_6
DATE:2026-05-03
1.cmake컴파일로 변경
2.html파일을 정적파일로 불러오기
3.정적파일 서빙 기능 추가
--------------------------
mini_webserver_7
DATE:2026-05-05
1.멀티서버
2.서버별 html문서 설정가능
3.서버별 라우트 설정가능
4.Servre listening 메세지 이후 CMD> 출력하도록 수정
5.디버깅 메세지 코멘트처리
--------------------------
mini_webserver_8
DATE:2026-05-09
1.GET,POST,PUT,DELETE,FETCH 메소드 지원
2.테스트는 GET,POST만 테스트
--------------------------
mini_webserver_9
DATE:2026-05-10
1.네임스페이스 적용
2.https적용(self-signed,openssl)
--------------------------
mini_webserver_10
DATE:2026-05-11
1.서버종료시에도 handle_client실행되는 문제 해결
--------------------------
mini_webserver_11
DATE:2026-05-12
1.가상호스트추가
2.서버종료부분에 try-catch추가
3.스레드 정리시간추가
--------------------------
mini_webserver_12
DATE:2026-05-13
1.리버스 프록시
--------------------------
mini_webserver_13
DATE:2026-05-14
1.코드 정리 - 기존과 기능은 동일함(mini_webserver_12).
--------------------------


2.yaml파일로 읽어 올경우 적용해야할것⭐️
- server:
  host: "localhost"
  port: 8080
  docroot : "public"
