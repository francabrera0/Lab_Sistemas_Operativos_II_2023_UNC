#!/bin/bash

filepath="/lib/systemd/system/webServer.service"
path=$PWD

touch "$filepath"

sudo echo "[Unit] 
Description=webServer

[Service]
Type=simple
Restart=always
RestartSec=5s
ExecStart=$path/bin/webServer
WorkingDirectory=$path/database
ProtectSystem=false
Environment=LABVI_API_SECRET=letmeinto
Environment=LABVI_TOKEN_HOUR_LIFESPAN=1

[Install]
WantedBy=multi-user.target" > "$filepath"
