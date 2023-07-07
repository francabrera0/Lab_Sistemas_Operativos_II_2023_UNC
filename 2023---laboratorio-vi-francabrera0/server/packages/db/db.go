package db

import (
	"database/sql"
	"fmt"
	"log"
	"time"
	"webServer/packages/model"

	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/bcrypt"
)

var database *sql.DB

// Connect with the database
// Create tables dor users and sensors (if they dont exists)
func Connect() error {
	db, err := sql.Open("sqlite3", "./users.db")

	if err != nil {
		return err
	}

	database = db

	statement, err := database.Prepare("CREATE TABLE IF NOT EXISTS USERS(id INTEGER PRIMARY KEY, USERNAME TEXT, PASSWORD TEXT, CREATED_AT TIMESTAMP)")
	if err != nil {
		fmt.Println(err)
	}

	statement.Exec()

	statement2, err := database.Prepare("CREATE TABLE IF NOT EXISTS SENSORS(id INTEGER PRIMARY KEY, USERNAME TEXT, TOTALMEMORY FLOAT, FREEMEMORY FLOAT, USEDMEMORY FLOAT, LASTUPDATE TIMESTAMP)")
	if err != nil {
		fmt.Println(err)
	}

	statement2.Exec()

	return nil
}

// Close connection
func Close() {
	database.Close()
}

// Save a new user into database
// For each user save id, username, password (encrypted) and creation date
func Save(u model.User) error {

	stm, err := database.Prepare("INSERT INTO USERS (USERNAME,PASSWORD,CREATED_AT) VALUES (?,?,?);")

	if err != nil {
		log.Println("Error in prepare query, Save function")
		log.Println(err)
		defer stm.Close()
		return err
	}

	defer stm.Close()

	safePass, err := bcrypt.GenerateFromPassword([]byte(u.Password), bcrypt.DefaultCost)

	if err != nil {
		log.Println("Error generating hash, Save function")
		log.Println(err)
		defer stm.Close()
		return err
	}

	createdAt := time.Now()

	_, err = stm.Exec(u.Username, safePass, createdAt)

	if err != nil {
		log.Println("Error inserting user, Save function")
		log.Println(err)
		defer stm.Close()
		return err
	}

	return nil
}

// Get a specific entry of USERS table with their USERNAME
func Get(u model.User) model.User {
	stm, err := database.Prepare("SELECT * FROM USERS WHERE USERNAME=?;")

	if err != nil {
		log.Println("Error in prepare query, get function")
		log.Println(err)
	}

	defer stm.Close()

	var result model.User

	err = stm.QueryRow(u.Username).Scan(&result.Id, &result.Username, &result.Password, &result.CreatedAt)

	if err != nil {
		log.Println("User not found, get function")
		log.Println(err)
	}
	return result

}

// Get a specific entry of USERS table with their Id
func GetById(uid uint) (model.User, error) {
	stm, err := database.Prepare("SELECT * FROM USERS WHERE id=?;")

	if err != nil {
		log.Println("Error in prepare query, get function")
		log.Println(err)
	}

	defer stm.Close()

	var result model.User

	err = stm.QueryRow(uid).Scan(&result.Id, &result.Username, &result.Password, &result.CreatedAt)

	if err != nil {
		log.Println("Error in query row, get function")
		log.Println(err)
	}
	return result, err

}

// Get all entries of the USERS table
func GetAllUsers() ([]model.User, error) {
	stm, err := database.Prepare("SELECT id,USERNAME,CREATED_AT FROM USERS;")

	if err != nil {
		log.Println("Error in prepare query, getAllUsers function")
		log.Println(err)
		return nil, err
	}

	defer stm.Close()

	rows, err := stm.Query()
	if err != nil {
		log.Println("Error in query, get all users function")
		log.Println(err)
		return nil, err
	}
	defer rows.Close()

	var users []model.User

	for rows.Next() {
		var user model.User

		err := rows.Scan(&user.Id, &user.Username, &user.CreatedAt)
		if err != nil {
			log.Println("Error in decod query, get all user function")
			log.Println(err)
			return nil, err
		}
		users = append(users, user)
	}

	return users, nil
}

// Delete a specific user with their Id
func DropUserById(Id int) {
	stm, err := database.Prepare("DELETE FROM USERS WHERE id = ?;")

	if err != nil {
		log.Println("Error in prepare query, drop user by id function")
		log.Println(err)
	}

	defer stm.Close()

	_, err = stm.Exec(Id)

	if err != nil {
		log.Println("Error deleting user")
		log.Println(err)
	}

}

// Save a new instance of sensor information into SENSORS table
func SaveInfo(i model.SensorInfo) error {

	stm, err := database.Prepare("INSERT INTO SENSORS (USERNAME, TOTALMEMORY, FREEMEMORY, USEDMEMORY, LASTUPDATE) VALUES (?, ?, ?, ?, ?);")

	if err != nil {
		log.Println("Error in prepare query, Save INFO function")
		log.Println(err)
		defer stm.Close()
		return err
	}

	defer stm.Close()

	timeStamp := time.Now()

	_, err = stm.Exec(i.Username, i.TotalMemory, i.FreeMemory, i.UsedMemory, timeStamp)

	if err != nil {
		log.Println("Error inserting user, Save function")
		log.Println(err)
		defer stm.Close()
		return err
	}
	return nil
}

// Get all entries of SENSORS table
func GetInfo() ([]model.SensorInfo, error) {
	stm, err := database.Prepare("SELECT USERNAME, TOTALMEMORY, FREEMEMORY, USEDMEMORY, LASTUPDATE FROM SENSORS ORDER BY LASTUPDATE DESC;")

	if err != nil {
		log.Println("Error in prepare query, getInfo function")
		log.Println(err)
		return nil, err
	}

	defer stm.Close()

	rows, err := stm.Query()
	if err != nil {
		log.Println("Error in query, getInfo function")
		log.Println(err)
		return nil, err
	}
	defer rows.Close()

	var info []model.SensorInfo

	for rows.Next() {
		var i model.SensorInfo

		err := rows.Scan(&i.Username, &i.TotalMemory, &i.FreeMemory, &i.UsedMemory, &i.LastUpdate)
		if err != nil {
			log.Println("Error in decod query, getInfo function")
			log.Println(err)
			return nil, err
		}
		info = append(info, i)
	}
	return info, nil
}
