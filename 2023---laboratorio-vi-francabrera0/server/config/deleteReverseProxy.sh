#!/bin/bash

sudo service nginx stop
sudo rm /etc/nginx/sites-available/dashboard.com /etc/nginx/sites-available/sensors.com
sudo rm /etc/nginx/sites-enabled/dashboard.com /etc/nginx/sites-enabled/sensors.com