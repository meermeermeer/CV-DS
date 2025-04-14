#!/bin/bash
#dit zijn alle bash commando's die tijdens het doen van de lessons zijn gebruikt. Vervolgens zijn deze Bash-commands in Ansible gezet.

#lesson 4
sudo apt install apt-transport
curl -fsSL ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64]
sudo apt install docker.io
sudo snap install docker
docker --version

#lesson 7
mkdir /home/ansible/docker
cd docker
touch Dockerfile
vi Dockerfile
FROM ubuntu:23.04
MAINTAINER ansible 
RUN apt-get update
CMD ["sh", "-c", "echo Test Test Test"]
docker build -t image7 /home/ansible/docker
docker run --name lessen-7 image7

#lesson 8
sudo apt-get update
sudo apt-get install docker-compose-plugin
sudo chmod +x /usr/local/bin/docker-compose
sudo nano docker-compose.yml

#lesson 9
sudo apt-get update
sudo apt install docker.io
sudo systemctl start docker
sudo systemctl enable docker
sudo docker pull mysql
sudo docker run -d -p0.0.0.0:80:80 mysql:latest
sudo Docker Swarm init --advertise-addr 10.24.17.61 #docker vm 1
sudo docker node ls
sudo docker service create --name HelloWorld alpine ping docker.com

#node toevoegen aan swarm
sudo docker swarm join-token -q manager #kopieer de token
sudo docker swarm join --token SWMTKN-1-60pornambwphf0ew0k5r7gpai5cduyourqiquz3jikn6a7m91u-aseevlaowjqymyzo44dgf9c0k 10.24.17.61:2377
sudo docker node demote 'docker-ID-node1'
sudo docker node demote 'docker-ID-node2' #nu 1 centrale manager en 2 nodes

#lesson 10
sudo docker network create -d overlay --attachable mynet
sudo docker network connect --ip 10.0.0.61 mynet lessen-7
sudo docker image ls
sudo docker network inspect mynet

sudo docker network disconnect test-overlay lessen-7
sudo docker network rm mynet
sudo docker network prune