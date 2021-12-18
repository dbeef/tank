#!/bin/sh -e

sudo echo -e "\
[Unit]\n\
Description=All the bloat to control this tank\n\
[Service]\n\
RemainAfterExit=yes\n\
Type=forking\n\
ExecStart=$PWD/tank-scripts/start.sh $PWD\n\
Nice=-20\n\
Restart=always\n\
[Install]\n\
WantedBy=multi-user.target\n\
" > /etc/systemd/system/tank.service

sudo systemctl enable tank

