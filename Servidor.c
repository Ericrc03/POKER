#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>  // Para manejar hilos
#include <unistd.h>
#include <time.h>

void EjecutarScript(MYSQL *conn, const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("No se pudo abrir el archivo %s\n", filename);
		exit(1);
	}
	
	char query[4096];
	char line[1024];
	
	query[0] = '\0';
	
	//Ejecución de script
	
	while (fgets(line, sizeof(line), file)) 
	{
		if (line[strlen(line) - 1] == '\n') 
		{
			line[strlen(line) - 1] = '\0';
		}
		strcat(query, line);
		
		if (strchr(line, ';')) 
		{
			if (mysql_query(conn, query)) 
			{
				printf("Error al ejecutar la consulta: %s\n", mysql_error(conn));
			} 
			else {
				printf("Consulta ejecutada\n");
			}
			query[0] = '\0';
		}
	}
	
	fclose(file);
}



// FUNCION REGISTER USER
void RegisterUser(MYSQL *conn, char *nombre, char *cuenta, char *contrasenya, int sock_conn, char *response) {
	
	sprintf(response, "SELECT * FROM Jugadores WHERE cuenta='%s';", cuenta);
	
	
	if (mysql_query(conn, response)) 
	{
		printf("Error al ejecutar consulta SELECT: %s\n", mysql_error(conn));
		strcpy(response, "1/ERROR");
	} 
	
	//si todo va bn
	
	else 
	{
		MYSQL_RES *res = mysql_store_result(conn);
		int nuevo_id = 1;
		
		if (res && mysql_num_rows(res) > 0) 
		{
			MYSQL_ROW row = mysql_fetch_row(res);
			if (row[0] != NULL) 
			{
				nuevo_id = atoi(row[0]) + 1;
			}
			mysql_free_result(res);
		}
		
		sprintf(response, "INSERT INTO Jugadores (id, nombre, cuenta, contrasenya, capital) VALUES (%d, '%s', '%s', '%s', 1000.00);", nuevo_id, nombre, cuenta, contrasenya);
		
		if (mysql_query(conn, response)) 
		{
			printf("Error al insertar nuevo usuario: %s\n", mysql_error(conn));
			strcpy(response, "1/ERROR");
		}
		
		else 
		{
			strcpy(response, "1/REGISTERED");
			
			printf("en el 1 registered");
			
		}
		
		
	}
	
}


// FUNCION LOGIN USER
void LoginUser(MYSQL *conn, char *cuenta, char *contrasenya, int sock_conn, char *response) {
	
	sprintf(response, "SELECT * FROM Jugadores WHERE cuenta='%s' AND contrasenya='%s';", cuenta, contrasenya);
	
	if (mysql_query(conn, response)) {
		
		printf("Error al ejecutar la consulta: %s\n", mysql_error(conn));
		strcpy(response, "ERROR");
		
	}
	
	else 
	{
		
		MYSQL_RES *res = mysql_store_result(conn);
		
		if (res == NULL) {
			
			printf("Error al obtener el resultado: %s\n", mysql_error(conn));
			strcpy(response, "ERROR");
		}
		
		else if (mysql_num_rows(res) > 0) {
			
			char nombre[20];
			strcpy(response, "2/LOGGED_IN");
			char query_nombre[100];
			sprintf(query_nombre, "SELECT nombre FROM Jugadores WHERE cuenta='%s' AND contrasenya='%s';", cuenta, contrasenya);
			
			if (mysql_query(conn, query_nombre) == 0) {
				
				MYSQL_RES *res_nombre = mysql_store_result(conn);
				
				if (res_nombre && mysql_num_rows(res_nombre) > 0) {
					
					MYSQL_ROW row = mysql_fetch_row(res_nombre);
					strcpy(nombre, row[0]);
					mysql_free_result(res_nombre);
				}
			}
		} 
		
		else 
		{
			strcpy(response, "2/LOGIN_FAILED/");
		}
		mysql_free_result(res);
	}
}

int main(int argc, char *argv[]) {
	
	// Inicializar MySQL
	MYSQL *conn;
	conn = mysql_init(NULL);
	
	if (conn == NULL) {
		printf("Error al crear la conexi\uffc3\uffb3n: %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	conn = mysql_real_connect(conn, "localhost", "root", "mysql", NULL, 0, NULL, 0);
	if (conn == NULL) {
		printf("Error al inicializar la conexi\uffc3\uffb3n: %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	
	
	EjecutarScript(conn, "POKER.sql");
	
	
	if (mysql_select_db(conn, "PokerDB") != 0) {
		printf("Error seleccionando la base de datos: %s\n", mysql_error(conn));
		exit(1);  // O manejar el error de otra forma
	}
	
	
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	char buff[512];
	char response[512];
	
	
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creando socket");
	
	
	int opt = 1;
	if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9014);
	
	
	
	if (bind(sock_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
		printf("Error en el bind");
	if (listen(sock_listen, 2) < 0)
		printf("Error en el listen");
	
	printf("Servidor a la espera...\n");
	
	while (1) {
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("Conexi\uffc3\uffb3n recibida\n");
		
		int ret = read(sock_conn, buff, sizeof(buff));
		
		if (ret > 0) 
		{
			buff[ret] = '\0';
			printf("Mensaje recibido: %s\n", buff);  // Imprimir el mensaje recibido para depuraci\uffc3\uffb3n
		} 
		
		else 
		{
			printf("Error al recibir datos\n");
			close(sock_conn);
			continue;  // Saltar a la siguiente iteraci\uffc3\uffb3n del bucle si hay error al recibir
		}
		// Procesar el mensaje
		char *p = strtok(buff, "/");
		
		// Comprobar si el cliente quiere desconectar
		if (strcmp(p, "0") == 0) 
		{
			printf("Cliente desconectado.\n");
			strcpy(response, "DISCONNECT");
			write(sock_conn, response, strlen(response));  // Enviar la confirmaci\uffc3\uffb3n de desconexi\uffc3\uffb3n al cliente
			close(sock_conn);
			continue;  // Saltar a la siguiente iteraci\uffc3\uffb3n del bucle
		}
		
		
		// register
		
		if (strcmp(p, "REGISTER") == 0) 
		{
			char *nombre = strtok(NULL, "/");
			char *cuenta = strtok(NULL, "/");
			char *contrasenya = strtok(NULL, "/");
			
			RegisterUser(conn, nombre, cuenta, contrasenya, sock_conn, response);
			
			printf("11");
			
			write(sock_conn, response, strlen(response)+1);  //le envio al usuario o registered o error
			
			
		}
		
		
		else if (strcmp(p, "LOGIN") == 0) 
		{
			char *cuenta = strtok(NULL, "/");
			char *contrasenya = strtok(NULL, "/");
			
			LoginUser(conn, cuenta, contrasenya, sock_conn, response);
			
			write(sock_conn, response, strlen(response) + 1);  
		}
		
		
	}
	
	
	mysql_close(conn);
	return 0;
}
