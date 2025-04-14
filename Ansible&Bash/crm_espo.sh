#!/bin/bash
sudo apt update && sudo apt upgrade
sudo apt install mariadb-server
sudo apt install docker.io

container_name="espocrm"

port_mapping="8080:80"

db_host="mysql_host"
db_port="3306"
db_name="espocrm"
db_user="espocrm_user"
db_password="password"

data_volume="/test/espo-crm"

sudo docker pull espocrm/espocrm:latest

sudo docker run -d --name $container_name \
  -p $port_mapping \
  -v $data_volume:/var/www/html/data \
  -e DB_HOST=$db_host \
  -e DB_PORT=$db_port \
  -e DB_NAME=$db_name \
  -e DB_USER=$db_user \
  -e DB_PASSWORD=$db_password \
  espocrm/espocrm:latest

echo "succes"

#na installatie kan espocrm via webbrowser verder geinstalleerd worden met credentials die hierboven staan.