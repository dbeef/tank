#!/bin/sh -e

sudo echo -e "\
[Unit]\n\
Description=All the bloat to control this tank\n\
[Service]\n\
Type=simple\n\
ExecStart=cd $PWD && source tank-scripts/start.sh\n\
Nice=600\n\
Restart=always\n\
[Install]\n\
WantedBy=multi-user.target\n\
" > /etc/systemd/system/tank.service

sudo systemctl enable tank

