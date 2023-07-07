BINARY_NAME=./bin/webServer
SOURCE=./cmd/webServer/main.go
BIN=./bin
DATABASE=./database

build:
	@echo "\e[32mBuilding...\e[0m"
	mkdir -p $(BIN)/
	go build -o $(BINARY_NAME) $(SOURCE)

run:
	@echo "\e[32mRunning...\e[0m"
	go run $(SOURCE)

clean:
	@echo "\e[32mCleaning...\e[0m"
	rm -rf $(BIN)

uninstall: stop
	@echo "\e[32mRemoving service file...\e[0m"
	-sudo sh ./config/uninstall.sh

stop: 
	@echo "\e[32mStopping...\e[0m"
	-sudo service webServer stop

start: build stop delnginx nginx

	@echo "\e[32mUpdating service file...\e[0m"
	sudo sh ./config/setup.sh
	mkdir -p $(DATABASE)
	@echo "\e[32mReloading...\e[0m"
	sudo systemctl daemon-reload
	@echo "\e[32mStarting...\e[0m"
	sudo service webServer start

deleteUsers: 
	@echo "\e[32mDelete user database...\e[0m"
	-sudo sh ./config/deleteUsers.sh
	rm -rf $(DATABASE)
	
status:
	sudo service webServer status

nginx:
	sudo sh ./config/reverseProxy.sh

delnginx:
	-sudo sh ./config/deleteReverseProxy.sh

cleanall: deleteUsers uninstall clean delnginx