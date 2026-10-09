// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	char send_buffer[BUFFER_SIZE] = {0};

	int port;
	char *endptr;

	if (argc == 1) {
		port = DEFAULT_PORT;
		printf("Cap port indicat. Utilitzant valor per defecte: %d\n", port);
	}
	else if (argc == 2) {
		errno = 0;
		port = strtol(argv[1], &endptr, 10);

		if (errno != 0 || *endptr != '\0' || port < 1 || port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "Ús: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Error en crear el socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Error en fer el bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	if (listen(sock, 3) < 0) {
		perror("Error en escoltar");
		close(sock);
		exit(EXIT_FAILURE);
	}

	printf("Servidor en funcionament, esperant connexions...\n");

	while (1) {

		if ((new_socket = accept(sock, (struct sockaddr *)&address,
		                         (socklen_t *)&addrlen)) < 0) {
			perror("Error en acceptar la connexió");
			close(sock);
			exit(EXIT_FAILURE);
		}

		while (1) {

			memset(buffer, 0, BUFFER_SIZE);
			memset(send_buffer, 0, BUFFER_SIZE);

			if (recv(new_socket, buffer, BUFFER_SIZE, 0) <= 0) {
				printf("Client desconnectat (PID: %d)\n", getpid());
				break;
			}

			printf("Missatge rebut del client (PID: %d): %s\n",
			       getpid(), buffer);

			/* OPCIÓ 6: SORTIR */
			if (strcmp(buffer, "EXIT") == 0) {

				printf("Tancant connexió amb el client (PID: %d)...\n",
				       getpid());

				send(new_socket, "Connexió tancada\n",
				     strlen("Connexió tancada\n"), 0);

				break;
			}

			/* OPCIÓ 2: TEMPS DE VOL */
			else if (strncmp(buffer, "FLIGHT_TIME", 11) == 0) {

				double distancia;
				double velocidad;
				double tiempo;

				if (sscanf(buffer, "FLIGHT_TIME %lf %lf",
				           &distancia, &velocidad) == 2) {

					if (velocidad > 0) {

						tiempo = distancia / velocidad;

						snprintf(send_buffer, BUFFER_SIZE,
						         "%.2f h\n", tiempo);

					} else {

						snprintf(send_buffer, BUFFER_SIZE,
						         "Error: la velocitat ha de ser major que 0.\n");
					}

				} else {

					snprintf(send_buffer, BUFFER_SIZE,
					         "Error: format incorrecte.\n");
				}
			}

			/* OPCIÓ 3: COMBUSTIBLE */
			else if (strncmp(buffer, "FUEL", 5) == 0) {

				double temps;
				double consum;
				double combustible;

				if (sscanf(buffer, "FUEL %lf %lf",
				           &temps, &consum) == 2) {

					if (temps >= 0 && consum >= 0) {

						combustible = temps * consum;

						snprintf(send_buffer, BUFFER_SIZE,
						         "%.2f L\n", combustible);

					} else {

						snprintf(send_buffer, BUFFER_SIZE,
						         "Error: els valors han de ser positius.\n");
					}
				} 
			// OPCIÓ 4: COST DEL COMBUSTIBLE
			else if (strncmp(buffer, "COST", 4) == 0) {
					
					double cost, combustible, preu;
					
					if (sscanf(buffer, "COST %lf %lf ", &preu, &combustible) == 2) {
						if (combustible >= 0 && preu >= 0) {
							cost = combustible * preu;
							snprintf(send_buffer, BUFFER_SIZE,"%.2f €\n", cost);
						} else {
							snprintf(send_buffer, BUFFER_SIZE,
							         "Error: els valors han de ser positius.\n");
						}

				} else {

					snprintf(send_buffer, BUFFER_SIZE,
					         "Error: valors incorrectes.\n");
				}
			}
			// OPCIÓ 5: RANGE
			else if (strncmp(buffer, "RANGE", 5)== 0){

				double combustible, consum, velocitat, temps, distancia;

				if (sscanf(buffer, "RANGE %lf %lf %lf ", &combustible, &consum, &velocitat) == 3) {
					
					if (combustible >= 0 && consum >= 0 && velocitat >= 0){

						temps = combustible / consum;
						distancia = temps * velocitat;

						snprintf(send_buffer, BUFFER_SIZE,"%.2f km\n", distancia);
					} else {
						snprintf(send_buffer, BUFFER_SIZE,
						         "Error: els valors han de ser positius.\n");

					}
			}


			/* MISSATGE NORMAL */
			else {

				snprintf(send_buffer, BUFFER_SIZE,
				         "Missatge rebut: %s\n", buffer);
			}

			send(new_socket, send_buffer,
			     strlen(send_buffer), 0);
		}

		close(new_socket);
	}
	}
	close(sock);
	return 0;
	}

}
