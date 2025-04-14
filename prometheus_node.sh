#!/bin/bash

# Step 1 - Update system packages
sudo apt update

# Step 2 - Download the latest Node Exporter release (adjust version if needed)
NODE_EXPORTER_VERSION="1.6.1"
wget https://github.com/prometheus/node_exporter/releases/download/v${NODE_EXPORTER_VERSION}/node_exporter-${NODE_EXPORTER_VERSION}.linux-amd64.tar.gz

# Step 3 - Extract Node Exporter
tar xvfz node_exporter-${NODE_EXPORTER_VERSION}.linux-amd64.tar.gz

# Step 4 - Move the Node Exporter binary to /usr/local/bin
sudo mv node_exporter-${NODE_EXPORTER_VERSION}.linux-amd64/node_exporter /usr/local/bin/

# Step 5 - Create a system user for Node Exporter (with no login)
sudo useradd -rs /bin/false node_exporter

# Step 6 - Create a systemd service file for Node Exporter
sudo tee /etc/systemd/system/node_exporter.service > /dev/null <<EOF
[Unit]
Description=Prometheus Node Exporter
Wants=network-online.target
After=network-online.target

[Service]
User=node_exporter
Group=node_exporter
Type=simple
ExecStart=/usr/local/bin/node_exporter

[Install]
WantedBy=multi-user.target
EOF

# Step 7 - Reload systemd and start the Node Exporter service
sudo systemctl daemon-reload
sudo systemctl enable node_exporter
sudo systemctl start node_exporter

# Step 8 - Check if Node Exporter is running
sudo systemctl status node_exporter

# Step 9 - Display where Node Exporter is available (port 9100)
echo "Node Exporter is running and available at http://localhost:9100/metrics"
