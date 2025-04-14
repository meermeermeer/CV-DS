#!/bin/bash

cp /etc/resolv.conf /etc/resolv.conf.backup

if grep -q "nameserver 8.8.8.8" /etc/resolv.conf; then   #DNS goedzetten wanner nodig
  echo "DNS 8.8.8.8 is already set."
else
  sed -i '/^nameserver /d' /etc/resolv.conf
  echo "nameserver 8.8.8.8" >> /etc/resolv.conf
  echo "DNS is gewijzigd naar 8.8.8.8."
fi


apt update
apt install ufw -y   #ufw installeren

ufw enable #ufw aan
ufw default deny incoming #inkomend verkeer blokkeren
ufw default allow outgoing #uigaand verkeer aanzetten

ufw allow ssh #ssh toelaten

ufw allow 80/tcp #poort 80 open

ufw allow 443/tcp #poort 443 open

ufw enable #ufw aan

ufw status verbose #status van ufw

echo "succes"