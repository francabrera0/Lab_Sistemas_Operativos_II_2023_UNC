server {
    listen 80;
    listen [::]:80;

    server_name sensors.com www.sensors.com;

    location / {

        proxy_pass http://localhost:8080/processing/;
        proxy_set_header Host $http_host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}