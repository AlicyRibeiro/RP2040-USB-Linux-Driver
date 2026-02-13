#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE "/dev/pico_usb"

/* Comandos do protocolo */
#define CMD_LED_ON    0x01
#define CMD_LED_OFF   0x02
#define CMD_LED_BLINK 0x03

int main(void)
{
    int fd;
    int option;
    uint8_t cmd[3];

    fd = open(DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("Erro ao abrir /dev/pico_usb");
        return 1;
    }

    printf("=== Pico USB Controller ===\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1 - LED ON\n");
        printf("2 - LED OFF\n");
        printf("3 - BLINK\n");
        printf("0 - SAIR\n");
        printf("Opcao: ");
        scanf("%d", &option);

        if (option == 0)
            break;

        switch (option) {
        case 1:
            cmd[0] = CMD_LED_ON;
            cmd[1] = 0x00;
            write(fd, cmd, 2);
            printf("LED ON enviado\n");
            break;

        case 2:
            cmd[0] = CMD_LED_OFF;
            cmd[1] = 0x00;
            write(fd, cmd, 2);
            printf("LED OFF enviado\n");
            break;

        case 3: {
            int vezes;
            printf("Quantas vezes piscar? ");
            scanf("%d", &vezes);

            cmd[0] = CMD_LED_BLINK;
            cmd[1] = 0x01;          // parâmetro (seu protocolo)
            cmd[2] = (uint8_t)vezes;

            write(fd, cmd, 3);
            printf("BLINK enviado (%d vezes)\n", vezes);
            break;
        }

        default:
            printf("Opcao invalida!\n");
            break;
        }
    }

    close(fd);
    printf("Aplicacao encerrada\n");
    return 0;
}
