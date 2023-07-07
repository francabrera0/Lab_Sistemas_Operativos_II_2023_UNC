package controller

import (
	"fmt"
	"log"
	"net/http"
	"webServer/packages/model"
	"webServer/packages/model/service"

	"github.com/gin-gonic/gin"
)

// POST
// Create a new user if the username is available
//
//	Check if the username already exists
//	Save username and password in the database
//	Create a new user in the linux system with the same credentials (available for ssh conections)
func RegisterUser(c *gin.Context) {
	var userReg model.User

	if err := c.ShouldBindJSON(&userReg); err != nil {
		log.Println("Invalid parameters in RegisterUser Function")
		log.Println(err)
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if !service.ValidateNewUser(userReg) {
		log.Println("Username is not available in RegisterUser Function")
		c.JSON(http.StatusConflict, gin.H{"error": "Username is not available"})
		return
	}

	err := service.NewUser(userReg)

	if err != nil {
		service.DropUser(userReg)
		c.JSON(http.StatusBadRequest, gin.H{"error": "Could not create user"})
	} else {
		message := fmt.Sprintf("%s %s", userReg.Username, "succesfully created")
		c.JSON(http.StatusOK, gin.H{"response": message})
	}

}

// POST
// Login, if the username and password are correct, it returns a jwt
//
//	jwt is valid until the time indicated in the enviroment variable "TOKEN_HOUR_LIFESPAN"
func Login(c *gin.Context) {
	var userReg model.User

	if err := c.ShouldBindJSON(&userReg); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	token, err := service.ValidateUser(userReg)

	if err != nil {
		c.JSON(http.StatusUnauthorized, gin.H{"error": "invalid username or password"})
		return
	}

	c.JSON(http.StatusOK, gin.H{"token": token})
}

// GET
// Return a list with all users
//
//	For each user, the id, username, and creation date are shown
//	Require authentication
func ListAll(c *gin.Context) {
	users, err := service.GetUsers()

	if err != nil {
		c.JSON(http.StatusBadRequest, err)
	}

	data := map[string]interface{}{
		"users": users,
	}
	c.JSON(http.StatusOK, data)

}
