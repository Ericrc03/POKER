DROP DATABASE IF EXISTS PokerDB;
CREATE DATABASE PokerDB CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
use PokerDB;
CREATE TABLE Mesa (id_mesa INTEGER PRIMARY KEY NOT NULL, num_jug INTEGER NOT NULL);
CREATE TABLE Jugadores (
	id INTEGER PRIMARY KEY NOT NULL, 
	nombre TEXT NOT NULL,
	capital DECIMAL(30,2) NOT NULL,
	cuenta TEXT NOT NULL, 
	contrasenya TEXT NOT NULL);

INSERT INTO Jugadores VALUES(101, 'Juan', 3000, 'juan@gmail.com', 'contraseña');	 
INSERT INTO Jugadores VALUES(102, 'Maria', 2500, 'maria@gmail.com', '1234');
INSERT INTO Jugadores VALUES(103, 'Luis', 3500, 'luis@gmail.com', 'luis123');
