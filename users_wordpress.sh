#!/bin/bash

# Voor Wordpress containers

# Prompt the user for a username for admin and normal user
read -p "Enter the username for the admin user: " admin_user    #username = admin/user. ww=proxmox123
read -p "Enter the username for the normal user: " normal_user

# Wachtwoord voor beide gebruikers
admin_password="proxmox"
normal_password="proxmox"

# Function to create a directory if it doesn't exist
make_directory() {
  local dir="$1"
  if [ ! -d "$dir" ]; then
    mkdir -p "$dir"
  fi
}

# Create a directory for SSH keys for the admin user if it doesn't exist
admin_ssh_dir="/home/$admin_user/.ssh"
make_directory "$admin_ssh_dir"

# Create a directory for SSH keys for the normal user if it doesn't exist
normal_ssh_dir="/home/$normal_user/.ssh"
make_directory "$normal_ssh_dir"

adduser --disabled-password --gecos "" "$admin_user"
echo "$admin_user:$admin_password" | chpasswd  # Stel wachtwoord in
usermod -aG "$admin_user" # Root rechten

adduser --disabled-password --gecos "" "$normal_user"
echo "$normal_user:$normal_password" | chpasswd  # Stel wachtwoord in

# Generate SSH key pairs
-u "$admin_user" ssh-keygen -t rsa -b 4096 -f "$admin_ssh_dir/id_rsa" -N ""
-u "$normal_user" ssh-keygen -t rsa -b 4096 -f "$normal_ssh_dir/id_rsa" -N ""

echo "SSH keys have been generated and set up for $admin_user and $normal_user."
