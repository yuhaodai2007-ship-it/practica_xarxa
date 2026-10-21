// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define DEFAULT_DOMAIN "localhost"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
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
	/* socket() funció per crear el socket*/ 
    /* AF_INET especifica la la familia de direcció, en aquest cas indica que el socket utiliza el protocol IPv4*/ 
    /* SOCK_STREAM especifica el tipus de socket que utiliza*/
    /* 0 especifica el tipus especific dins de la familia de direcció, en aquest cas intendifica com*/ 
		printf("\nError en crear el socket\n");
		return -1;
	}

	// Afegiu comentari sobre què són aquests 3 paràmetres,
	// per què s'utilitzen els valors que hi ha,
	// i per què s'utilitza htons()

	serv_addr.sin_family = AF_INET;
	/* Espesifica la familia d'adreça de la xarxa d'aquesta estructura */
	serv_addr.sin_port = htons(port);
	/* defineix el número de port en el servidor que acceptarà */
    /* 'port' serveix per determinar el punt d'accés la */
    /* converteix un sencer de 16 bits de l'ordre de bytes de l'host a l'ordre de xarxa (Big-Endian)
    És imprescindible perquè la xarxa interpreti correctament el número de port sense invertir els bytes */
	inet_pton(AF_INET, domini, &serv_addr.sin_addr);	// Convertir adreça IPv4 a binari
   

	// Connectar amb el servidor
	// Afegiu comentari explicant els arguments
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	/* connect() inicia la connexió activa des del nostre socket client ('sock') cap al servidor. */ 
    /* L'estructura 'serv_addr' (passada com a punter genèric) conté la IP i port de destí, i n'indiquem la mida. */ 
	/* L'argument (struct sockaddr *) actua com a punter genèric per suportar diversos protocols. */
    /* sock: És el descriptor de fitxer del socket que hem creat prèviament.*/
    /* (struct sockaddr *)&address: Punter a la configuració d'adreça i port. Fem 
        un 'cast' (conversió de tipus) cap a 'struct sockaddr *' perquè la funció bind() 
        és genèrica i cal adaptar la nostra estructura 'sockaddr_in' (específica d'IPv4) al format genèric. */
    /* sizeof(address): La mida en bytes de l'estructura passada. */
		printf("\nConnexio fallida\n");
		return -1;
	}

	printf("Connectat al servidor. Podeu començar a enviar missatges.\n");

	int option;

	while (1) {
		memset(buffer, 0, sizeof(buffer)); // Netegem el buffer

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
			/* send() transmet les dades al destí: 'sock' és el descriptor de la connexió i 'cadena' les dades.*/ 
			/*'strlen(cadena)' indica el nombre exacte de bytes a enviar i '0' aplica el comportament estàndard (sense flags).*/ 

			// Llegir resposta del servidor
			// Afegiu un control d'errors al recv()
			// Afegiu comentari explicant què fa i per què s'utilitza memset
			// Afegiu comentari explicant els arguments de la crida a recv()

			memset(buffer, 0, BUFFER_SIZE);
			/* memset() omple el 'buffer' amb zeros per esborrar dades de lectures anteriors i 
    		garantir que el nou text quedi correctament finalitzat amb un caràcter nul ('\0'). */ 
			recv(sock, buffer, BUFFER_SIZE,0);
			/*recv() llegeix la resposta des del 'sock' i la desa al 'buffer'. Utilitzem 'BUFFER_SIZE' 
			per limitar la lectura i evitar desbordaments de memòria. El '0' indica comportament estàndard.*/
			printf("Resposta del servidor: %s\n", buffer);

			if (strcmp(cadena, "EXIT") == 0) {
				close(sock);
				return 0;
			}
			break;

		case 2:
			// Implementar Opció 2
			break;

		case 3:
			// Implementar Opció 3
			break;

		case 4:
			// Implementar Opció 4
			break;

		case 5:
			// Implementar Opció 5
			break;

		default:
			printf("Opció invàlida\n");
			break;
		}
	}

	close(sock);
	return 0;
}
