
# RP2040 USB Linux Driver

Este projeto implementa um **driver USB genérico no Linux**, utilizando uma placa **Raspberry Pi Pico (RP2040)** operando em modo **periférico USB**.

O trabalho demonstra a comunicação completa entre:
- **Firmware embarcado (RP2040 + TinyUSB)**
- **Driver em Kernel Space (Linux USB Driver)**
- **Aplicação em Espaço de Usuário (User Space App)**

O projeto foi desenvolvido como parte da disciplina **Técnicas de Programação para Sistemas Embarcados II (TPSE II)**.

---

##  Objetivos do Projeto

- Implementar um **driver USB no Linux** baseado no modelo de dispositivos genéricos.
- Criar um **dispositivo de caractere (/dev/pico_usb)**.
- Permitir comunicação **user space → kernel space → dispositivo USB**.
- Demonstrar **operações de read/write** no driver.
- Integrar firmware, driver e aplicação de usuário.

---

##  Arquitetura do Projeto

````
 ├── firmware_pico/ # Firmware RP2040 (TinyUSB)
 │ ├── main.c
 │ └── CMakeLists.txt
 │
 ├── driver_linux/ # Driver USB no Linux (Kernel Space)
 │ ├── pico_usb_driver.c
 │ ├── Makefile
 │
 ├── app_user/ # Aplicação em Espaço de Usuário
 │ ├── app_user.c
 │ └── app
 ├── docs/
 │   ├── configuracao_inicial.md
 │   └── demonstracao_video.mp4
 │   └── relatorio_tecnico.pdf
 │
 ├── README.md
 └── LICENSE
````

---


##  Funcionamento Geral

1. O **RP2040** executa um firmware USB Vendor Class usando **TinyUSB**.
2. O dispositivo USB é enumerado pelo Linux com:
     * **VID 0xCAFE**
     *  **PID 0x4001**.
4. O **driver Linux** reconhece o dispositivo
5. O driver cria o dispositivo de caractere:
```
/dev/pico_usb
````

4. A **aplicação de usuário** envia comandos via `write()` para o driver.
5. O driver envia os dados ao RP2040 usando **transferências USB Bulk**.
6. O firmware interpreta os comandos e controla o **LED da placa**.

---

##  Protocolo de Comandos

| Comando | Descrição |
|------|---------|
| `0x01 0x00` | Liga o LED |
| `0x02 0x00` | Desliga o LED |
| `0x03 0x01 N` | Pisca o LED N vezes |

Exemplo em terminal:
```bash
printf "\x01\x00" | sudo tee /dev/pico_usb
````

## Driver Linux (Kernel Space)

O driver implementa:
* Registro como USB driver
* Criação de dispositivo de caractere
* Operações:
    * open
    * read
    * write

Exemplo da função write:

````
static ssize_t pico_write(struct file *file,
                          const char __user *buffer,
                          size_t len,
                          loff_t *offset)
{
    printk(KERN_INFO "pico_usb: comando recebido\n");
    ...
}
````
Logs podem ser acompanhados em tempo real com:
````
sudo dmesg -w
````

Isso demonstra depuração em kernel space.

---

## Firmware RP2040

O firmware foi desenvolvido com:
* Pico SDK
* TinyUSB (Vendor Class)
* Comunicação via endpoint Bulk


Os comandos recebidos são armazenados em uma fila circular e processados no loop principal, garantindo:

* Comunicação não bloqueante
* Separação entre interrupção USB e lógica de aplicação
* Temporização não bloqueante usando `absolute_time_t`


---
## Aplicação de Usuário

A aplicação em C permite controlar o dispositivo através de um menu interativo, utilizando chamadas de sistema padrão (`open`, `write`, `close`).

Compilação:
````
gcc app_user.c -o app
````

Execução:
````
sudo ./app
````

Também é possível testar diretamente pelo terminal usando printf.

---

## Observação Importante – Secure Boot

Em alguns sistemas Linux com Secure Boot habilitado, pode não ser possível carregar módulos externos do kernel.

Caso ocorra erro ao executar:

````
sudo insmod pico_usb_driver.ko
````
Pode ser necessário:
  * Desativar o Secure Boot na BIOS/UEFI
        ou
  * Assinar digitalmente o módulo do kernel

Durante o desenvolvimento deste projeto, pode ter sido necessário desativar temporariamente o Secure Boot para permitir o carregamento do driver.


---

## Demonstração em Vídeo

O vídeo demonstra:
* Compilação do firmware
* Carregamento do driver Linux
* Upload para o RP2040
* Carregamento do módulo do kernel
* Criação do dispositivo `/dev/pico_usb`
* Envio de comandos via aplicação de usuário
* Logs do kernel com `dmesg`
* Funcionamento do LED

O vídeo está disponível no link abaixo:
 - [Demonstração em Vídeo](docs/demonstracao_video.md)

---

## Relatório Técnico

O repositório inclui um Relatório Técnico em PDF, contendo:

* Introdução teórica
* Arquitetura detalhada
* Implementação do driver
* Implementação do firmware
* Aplicação de usuário
* Testes e validação
* Considerações sobre Secure Boot

Arquivo disponível em:
````

docs/relatorio_tecnico.pdf
````


---

## Tecnologias Utilizadas

* Raspberry Pi Pico (RP2040)
* TinyUSB
* Linux Kernel Module
* USB Bulk Transfer
* Linguagem C
* GCC
* Makefile
* CMake

---

## Autora

Ana Alicy Ribeiro

Curso: Engenharia de Computação

Disciplina: TPSE II

---

## Licença
Este projeto está licenciado sob a licença MIT.

##  Documentação Adicional

- [Configurações Iniciais](docs/configuracao_inicial.md)
- [Demonstração em Vídeo](docs/demonstracao_video.md)
- [Relatório Técnico](docs/relatorio_tecnico.pdf)
