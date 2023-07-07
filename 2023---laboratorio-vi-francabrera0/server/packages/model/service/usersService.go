package service

import (
	"errors"
	"fmt"
	"log"
	"os/exec"
	"strings"
	"webServer/packages/db"
	"webServer/packages/model"
	"webServer/packages/token"

	"golang.org/x/crypto/bcrypt"
)

// Check if the username already exists
func ValidateNewUser(u model.User) bool {
	queryUser := db.Get(u)
	if queryUser.Username == "" {
		return true
	}
	return false
}

// Save a new user in the db and create a OS user
func NewUser(u model.User) error {
	err := db.Save(u)

	if err != nil {
		return err
	}

	cmd := exec.Command("sudo", "useradd", "-m", u.Username, "-s", "/bin/bash")
	err = cmd.Run()

	if err != nil {
		log.Println("Error creating new user, new user function")
		log.Println(err)
		return err
	}

	cmd = exec.Command("sudo", "chpasswd")
	cmd.Stdin = strings.NewReader(fmt.Sprintf("%s:%s", u.Username, u.Password))
	err = cmd.Run()
	if err != nil {
		log.Println("Error creating new user, new user function")
		log.Println(err)
		return err
	}

	return nil
}

// Verify that the username and password are correct
func ValidateUser(u model.User) (string, error) {
	queryUser := db.Get(u)

	if queryUser.Username != "" {
		err := bcrypt.CompareHashAndPassword([]byte(queryUser.Password), []byte(u.Password))
		if err != nil {
			log.Println("Invalid user")
			log.Println(err)
			return "", err
		}
		token, err := token.GenerateToken(uint(u.Id))
		return token, err
	}
	log.Println("Username dos not exists, ValidateUser function")
	return "", errors.New("The username does not exists")
}

// Return a slice with all users
func GetUsers() ([]model.User, error) {
	return db.GetAllUsers()
}

// Return a specific user with their uid
func GetUserByID(uid uint) (model.User, error) {
	return db.GetById(uid)
}

// Delete an user
func DropUser(u model.User) {
	db.DropUserById(u.Id)
}
