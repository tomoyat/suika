# 学習用TCP/IPのプロトコルスタック

## clion

clionで実行する場合はconfigurationのrun with root privilegesをcheckを入れる

## 便利コマンド

```shell
set dpid (docker ps | grep -e "cmake" -e "gdb" | head -n 1 | awk '{print $1}'); docker exec -it {$dpid} bach -c "ping 192.0.2.2"
```

fish用
