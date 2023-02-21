# 게임 서버

## 설치 및 실행
Linux 런타임에서만 실행 가능하며 Apple Sillicon호스트를 기준으로 설명

</br>

### Docker 설치
https://www.docker.com/

</br>

### xmake 설치
https://xmake.io/#/getting_started

</br>

### Remote Build Server 실행
```
./install
```

</br>

### 게임 서버 실행
```
xmake service --connect
xmake build -v --root -y
xmake run --root 

```

만약 "server unreachable"같은 에러가 발생하면 연결 초기화 후 다시 진행
```
rm -rf .xmake
xmake service --disconnect
xmake service --connect
```
xmake의 버그로 run 도중에 Ctrl+c로 실행을 종료하더라도 프로세스가 남아 있기 때문에 다시 실행하려면 docker exec -it ...로 직접 kill 해야함