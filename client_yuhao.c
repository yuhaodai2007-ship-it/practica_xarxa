// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define DEFAULT_DOMAIN "127.0.0.1"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	char send_buffer[BUFFER_SIZE] = {0};
	char cadena[BUFFER_SIZE] = "";

	char *domini;
	int port;
	char *endptr;

	if (argc == 1) {
		// Cap argument -> valors per defecte
		domini = DEFAULT_DOMAIN;
		port = DEFAULT_PORT;
		printf("Cap argument indicat. Utilitzant valors per defecte: %s %d\n", domini, port);
	} 
	else if (argc == 3) {
		domini = argv[1];

		errno = 0;
		port = strtol(argv[2], &endptr, 10);

		// Comprovacions de validesa
		if (errno != 0 || *endptr != '\0' || port < 1 || port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
	} 
	else {
		fprintf(stderr, "Ús: %s [nom_de_domini port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}



	// Crear el socket
	// Afegiu comentari explicant els arguments, i de quines altres opcions hi ha
	// per al segon d'ells (ara SOCK_STREAM)
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\nError en crear el socket\n");
		return -1;
	}

	// Afegiu comentari sobre què són aquests 3 paràmetres,
	// per què s'utilitzen els valors que hi ha,
	// i per què s'utilitza htons()

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_pton(AF_INET, domini, &serv_addr.sin_addr);	// Convertir adreça IPv4 a binari
   

	// Connectar amb el servidor
	// Afegiu comentari explicant els arguments
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnexio fallida\n");
		return -1;
	}

	printf("Connectat al servidor. Podeu començar a enviar missatges.\n");

	int option;

	while (1) {
		memset(buffer, 0, sizeof(buffer)); // Netegem el buffer
		memset(send_buffer, 0, sizeof(send_buffer));

		// Menú principal
		// Cal que implementeu un petit servei remot amb almenys 4 funcionalitats noves
		// Definiu vosaltres mateixos les dades a enviar (demanar en el client, i analitzar al servidor) i rebre
		// Si per donar més sentit al servei cal alguna opció més, la podeu afegir
		// Deixeu l'opció 1 com a Enviar missatge i l'última per Sortir

		printf("Menú principal:\n");
		printf("1. Enviar missatge\n");
		printf("2. Opció 2\n");
		printf("3. Opció 3\n");
		printf("4. Opció 4\n");
		printf("5. Opció 5\n");
		printf("6. Sortir\n");
		printf("Opció: ");

		scanf("%d", &option);
		while (getchar() != '\n');  // buida el buffer fins al salt de línia

		switch (option)
		{
		case 1:
		case 6:
			if (option==1){
				printf("Introdueix el missatge a enviar ('EXIT' per tancar el servidor i sortir): ");
				fgets(cadena, BUFFER_SIZE, stdin);
				cadena[strcspn(cadena, "\n")] = '\0';  // Eliminar \n final
			} else {
				strcpy(cadena, "EXIT");
			}

			// Enviar missatge al servidor
			// Afegiu control d'errors
			// Afegiu comentari explicant els arguments

			send(sock, cadena, strlen(cadena), 0);

			// Llegir resposta del servidor
			// Afegiu un control d'errors al recv()
			// Afegiu comentari explicant què fa i per què s'utilitza memset
			// Afegiu comentari explicant els arguments de la crida a recv()

			memset(buffer, 0, BUFFER_SIZE);
			recv(sock, buffer, BUFFER_SIZE,0);
			printf("Resposta del servidor: %s\n", buffer);

			if (strcmp(cadena, "EXIT") == 0) {
				close(sock);
				return 0;
			}
			break;

		case 2:
		{
			// Implementar Opció 2
			double distancia, velocidad;

			printf("Introdueix la distància (km): ");
    		scanf("%lf", &distancia);

    		printf("Introdueix la velocitat (km/h): ");
    		scanf("%lf", &velocidad);

    		snprintf(cadena, BUFFER_SIZE, "1_FLIGHT_TIME %.2f %.2f", distancia, velocidad);

    		if (send(sock, cadena, strlen(cadena), 0) < 0) {
        		perror("Error en enviar");
        		break;
    		}

    		memset(buffer, 0, BUFFER_SIZE);

    		if (recv(sock, buffer, BUFFER_SIZE - 1, 0) < 0) {
        		perror("Error en rebre");
        		break;
    		}

    		printf("Resposta del servidor: %s\n", buffer);


			break;
		}
		case 3:
		{
    		double temps, consum;

			printf("Introdueix el temps de vol (h): ");
			scanf("%lf", &temps);

			printf("Introdueix el consum (L/h): ");
    		scanf("%lf", &consum);

			snprintf(cadena, BUFFER_SIZE, "2_FUEL %.2f %.2f", temps, consum);

			if (send(sock, cadena, strlen(cadena), 0) < 0) {
        		perror("Error en enviar");
        		break;
    		}

			memset(buffer, 0, BUFFER_SIZE);

			if (recv(sock, buffer, BUFFER_SIZE - 1, 0) < 0) {
				perror("Error en rebre");
				break;
			}

			printf("Resposta del servidor: %s\n", buffer);
			break;
		}

		case 4:
		{
			// Implementar Opció 4
			float combustible,preu;
			printf("Introdueix la quantitat de combustible (L): ");
			scanf("%f", &combustible);
			printf("Introdueix el preu del combustible (€/L): ");
			scanf("%f", &preu);
			while (getchar() != '\n');
			snprintf(cadena, BUFFER_SIZE, "3_COST %.2f %.2f", combustible, preu);

			if (send(sock, cadena, strlen(cadena), 0) < 0) {
        		perror("Error en enviar");
        		break;
    		}

			memset(buffer, 0, BUFFER_SIZE);

			if (recv(sock, buffer, BUFFER_SIZE - 1, 0) < 0) {
				perror("Error en rebre");
				break;
			}

			printf("Resposta del servidor: %s\n", buffer);
	
			break;
		}

		 case 5:
        {
            float combustible, consum, velocitat;
			printf("Introdueix la quantitat de combustible (L): ");
			scanf("%f", &combustible);
			printf("Introdueix el consum (L/h): ");
			scanf("%f", &consum);
			printf("Introdueix la velocitat (km/h): ");
			scanf("%f", &velocitat);
			while (getchar() != '\n');
			snprintf(cadena, BUFFER_SIZE, "4_RANGE %.2f %.2f %.2f", combustible, consum, velocitat);

			if (send(sock, cadena, strlen(cadena), 0) < 0) {
				perror("Error en enviar");
				break;
			}

			memset(buffer, 0, BUFFER_SIZE);

			if (recv(sock, buffer, BUFFER_SIZE - 1, 0) < 0) {
				perror("Error en rebre");
				break;
			}

			printf("Resposta del servidor: %s\n", buffer);
			// Implementar Opció 5
            break;
        }

        default:
            printf("Opció invàlida\n");
            break;
        }
    }

    close(sock);
    return 0;
}
