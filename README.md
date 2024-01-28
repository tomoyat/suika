# Suika

Small protocol stack for learning.
It has been created with reference to materials available at [this site](https://drive.google.com/drive/folders/1k2vymbC3vUk5CTJbay4LLEdZ9HemIpZe).

## run udp echo server

```shell
$ docker build -t suika ./
$ docker run --privileged --rm -it -v ./:/app suika:latest /bin/bash

# in docker
$ cmake /app
$ make
$ ./suika

# another terminal
$ docker exec -it xxxxxx /bin/bash
$ nc -u 192.0.2.2 5555
```
