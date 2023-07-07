#!/bin/bash

sudo cp ./config/dashboard.com /etc/nginx/sites-available
sudo ln -s /etc/nginx/sites-available/dashboard.com /etc/nginx/sites-enabled/dashboard.com

sudo cp ./config/sensors.com /etc/nginx/sites-available
sudo ln -s /etc/nginx/sites-available/sensors.com /etc/nginx/sites-enabled/sensors.com

sudo nginx -t
sudo systemctl restart nginx

