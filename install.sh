#!/bin/bash

docker build -t rxmake:0.1.1 docker/
docker run -d -p 9691:9691 -p 9988:9988 --name rxmake10 rxmake:0.1.1 /entrypoint.sh

mkdir -p ~/.xmake/service
cp docker/client.conf ~/.xmake/service/client.conf