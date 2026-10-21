
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  
#include <errno.h>

#define DEFAULT_PORT 8080
#define DEFAULT_DOMAIN "127.0.0.1"
#define BUFFER_SIZE 1024

/* Enviar una peticio i mostrar la resposta del servidor */
int enviar_i_rebre(int sock, const char *peticio) {
    char buffer[BUFFER_SIZE];

    if (send(sock, peticio, strlen(peticio), 0) < 0) {
        perror("Error en enviar");
        return -1;
    }

    memset(buffer, 0, sizeof(buffer));

    ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (n <= 0) {
        if (n < 0) {
            perror("Error en rebre");
        } else {
            printf("El servidor ha tancat la connexio.\n");
        }
        return -1;
    }

    buffer[n] = '\0';
    printf("Resposta del servidor: %s", buffer);

    return 0;
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;

    char *domini;
    int port;
    char *endptr;

    if (argc == 1) {
        // Cap argument -> valors per defecte
        domini = DEFAULT_DOMAIN;
        port = DEFAULT_PORT;
        printf("Utilitzant valors per defecte: %s %d\n",
               domini, port);
    } else if (argc == 3) {
        domini = argv[1];

        errno = 0;
        port = strtol(argv[2], &endptr, 10);
        // Comprovacions de validesa
        if (errno != 0 || *endptr != '\0' ||
            port < 1 || port > 65535) {
            fprintf(stderr, "Error: port incorrecte.\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Us: %s [nom_de_domini port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Crear el socket
	// Afegiu comentari explicant els arguments, i de quines altres opcions hi ha
	// per al segon d'ells (ara SOCK_STREAM)
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Error en crear el socket");
        return EXIT_FAILURE;
    }
    // Afegiu comentari sobre què són aquests 3 paràmetres,
	// per què s'utilitzen els valors que hi ha,
	// i per què s'utilitza htons()
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, domini, &serv_addr.sin_addr) != 1) {
        fprintf(stderr, "Error: adreca IPv4 incorrecta.\n");
        close(sock);
        return EXIT_FAILURE;
    }

    // Connectar amb el servidor
	// Afegiu comentari explicant els arguments
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
        perror("Connexio fallida");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("Connectat al servidor.\n");

    int option;

    while (1) {
        char peticio[BUFFER_SIZE];
        double distancia, velocitat, temps;
        double consum, combustible, preu;
        memset(buffer, 0, sizeof(buffer)); // Netegem el buffer
        
		// Menú principal
		// Cal que implementeu un petit servei remot amb almenys 4 funcionalitats noves
		// Definiu vosaltres mateixos les dades a enviar (demanar en el client, i analitzar al servidor) i rebre
		// Si per donar més sentit al servei cal alguna opció més, la podeu afegir
		// Deixeu l'opció 1 com a Enviar missatge i l'última per Sortir

        printf("\nMenu principal:\n");
        printf("1. Enviar missatge\n");
        printf("2. Calcular temps de vol\n");
        printf("3. Calcular combustible necessari\n");
        printf("4. Calcular cost del combustible\n");
        printf("5. Calcular autonomia\n");
        printf("6. Sortir\n");
        printf("Opcio: ");

        if (scanf("%d", &option) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Opcio incorrecta.\n");
            continue;
        }

        /* Netejar el salt de linia pendent */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        switch (option) {
            case 1: {
                char missatge[BUFFER_SIZE];

                printf("Introdueix el missatge: ");
				//
                if (fgets(missatge, sizeof(missatge), stdin) == NULL) {
                    printf("Error en llegir el missatge.\n");
                    break;
                }

                missatge[strcspn(missatge, "\n")] = '\0';

                if (strcmp(missatge, "EXIT") == 0) {
                    printf("EXIT esta reservat per sortir. "
                           "Utilitza l'opcio 6.\n");
                    break;
                }

                if (enviar_i_rebre(sock, missatge) < 0) {
                    close(sock);
                    return EXIT_FAILURE;
                }
                break;
            }
          
            case 2:
				// opció 2: calcular la distancia
                printf("Introdueix la distancia (km): ");
                if (scanf("%lf", &distancia) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }

                printf("Introdueix la velocitat (km/h): ");
                if (scanf("%lf", &velocitat) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }
                while ((c = getchar()) != '\n' && c != EOF) {}

                snprintf(peticio, sizeof(peticio),
                         "FLIGHT_TIME %.2f %.2f",
                         distancia, velocitat);
				// envia 

                if (enviar_i_rebre(sock, peticio) < 0) {
                    close(sock);
                    return EXIT_FAILURE;
                }
                break;

            case 3:
				// opció 3: calcular el combustible
                printf("Introdueix el temps de vol (h): ");
                if (scanf("%lf", &temps) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }

                printf("Introdueix el consum (L/h): ");
                if (scanf("%lf", &consum) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }
                while ((c = getchar()) != '\n' && c != EOF) {}

                snprintf(peticio, sizeof(peticio),
                         "FUEL %.2f %.2f", temps, consum);

                if (enviar_i_rebre(sock, peticio) < 0) {
                    close(sock);
                    return EXIT_FAILURE;
                }
                break;

            case 4:
				// opció 4: calcular el preu de cost del combustible
                printf("Introdueix el combustible (L): ");
                if (scanf("%lf", &combustible) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }

                printf("Introdueix el preu per litre (euros): ");
                if (scanf("%lf", &preu) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }
                while ((c = getchar()) != '\n' && c != EOF) {}

                snprintf(peticio, sizeof(peticio),
                         "FUEL_COST %.2f %.2f",
                         combustible, preu);

                if (enviar_i_rebre(sock, peticio) < 0) {
                    close(sock);
                    return EXIT_FAILURE;
                }
                break;

            case 5:
				// opció 5: calcular la distancia
                printf("Introdueix el combustible disponible (L): ");
                if (scanf("%lf", &combustible) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }

                printf("Introdueix el consum (L/h): ");
                if (scanf("%lf", &consum) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }

                printf("Introdueix la velocitat (km/h): ");
                if (scanf("%lf", &velocitat) != 1) {
                    printf("Valor incorrecte.\n");
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    break;
                }
                while ((c = getchar()) != '\n' && c != EOF) {}

                snprintf(peticio, sizeof(peticio),
                         "RANGE %.2f %.2f %.2f",
                         combustible, consum, velocitat);

                if (enviar_i_rebre(sock, peticio) < 0) {
                    close(sock);
                    return EXIT_FAILURE;
                }
                break;

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
            
            default:
                printf("Opcio invalida.\n");
                break;
        }
    }

    close(sock);
    return 0;
}
