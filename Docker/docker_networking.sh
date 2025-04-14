#!/bin/bash
sudo docker network create -d overlay --attachable mynet
sudo docker network connect --ip 10.0.0.61 mynet lessen-7
sudo docker image ls
sudo docker network inspect mynet

sudo docker network disconnect mynet lessen-7
sudo docker network rm mynet
sudo docker network prune