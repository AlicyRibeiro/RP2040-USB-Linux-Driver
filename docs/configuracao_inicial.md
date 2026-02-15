# Configuração Inicial do Ambiente

Este documento descreve as etapas necessárias para configurar o ambiente de desenvolvimento utilizado no projeto de implementação do driver USB para o RP2040.

---

# Sistema Operacional

O desenvolvimento foi realizado em ambiente Linux (Ubuntu ou compatível), devido ao suporte nativo para:

- Compilação de módulos do kernel
- Ferramentas de build
- SDK do RP2040

---

# Instalação de Dependências

Atualize o sistema:

````
sudo apt update
sudo apt upgrade
````

Instale as ferramentas essenciais:
````
sudo apt install build-essential cmake git gcc-arm-none-eabi \
libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib \
libusb-1.0-0-dev linux-headers-$(uname -r)
````

Esses pacotes fornecem:

- Compilador ARM para o RP2040
- Ferramentas de build
- Headers do kernel (necessários para o driver)

---

# Instalação do Pico SDK

Clone o SDK:
````
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
````

Inicialize os submódulos:
````
cd pico-sdk
git submodule update --init

````

Configure a variável de ambiente:
````
echo 'export PICO_SDK_PATH=~/pico-sdk' >> ~/.bashrc
source ~/.bashrc
````

---

# Configuração de Permissões USB

Para evitar executar tudo como root, crie uma regra udev:
````
sudo nano /etc/udev/rules.d/99-pico.rules
````

Adicione:
````
SUBSYSTEM=="usb", ATTR{idVendor}=="2e8a", MODE="0666"
````
Recarregue:
````
sudo udevadm control --reload-rules
sudo udevadm trigger
````

---

# Estrutura do Projeto

O projeto foi organizado da seguinte forma:

````
rp2040-usb-linux-driver/
├── driver_linux/      → módulo do kernel
├── firmware_pico/     → firmware embarcado
├── app_user/          → aplicação de usuário
└── docs/              → documentação

````

---

# Compilação do Firmware

````
cd firmware_pico
mkdir build
cd build
cmake ..
make
````

O arquivo .uf2 gerado deve ser copiado para a placa Pico em modo BOOTSEL.

---

# Compilação do Driver Linux
````
cd driver_linux
make
````

Carregar o módulo:
````
sudo insmod pico_usb_driver.ko
````

Verificar logs:
````
dmesg | tail
````

---

# Teste de Comunicação

Após conectar a placa, verifique se o dispositivo foi criado:
````
ls /dev/pico_usb
````

Enviar comando:
````
printf "\x01\x00" | sudo tee /dev/pico_usb
````

---

# Resultado Esperado

- Driver reconhece o dispositivo USB
- Arquivo /dev/pico_usb é criado
- Comandos enviados controlam o LED

---

# Observações

O driver deve ser recompilado após atualização do kernel

Certifique-se de que o firmware está carregado antes de testar

Logs do kernel podem ser acompanhados com:
````
dmesg -w
````
