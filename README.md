# Projeto Fetin 2024

O Projeto SHERLOCK visa automatizar o processo de retirada de produtos adquiridos em e-commerce, oferecendo uma solução prática e segura para os clientes. O conceito é simples: o cliente final recebe um QR Code, que é utilizado para abrir uma gaveta onde seu produto está armazenado. Essas gavetas fazem parte de armários inteligentes distribuídos em locais estratégicos, como faculdades, mercados e outros locais que oferecem segurança. Assim, pessoas que têm dificuldade em receber suas encomendas em casa podem optar por retirar seus produtos nesses armários, enquanto os entregadores utilizam um QR Code diferente para depositar as mercadorias nas gavetas designadas.

De forma simplificada, o sistema funciona assim: o QR Code é escaneado por uma câmera ESP32-CAM, que envia os dados para um dispositivo ESP32. Este, por sua vez, processa a informação e verifica sua validade com base em um banco de dados. Se o código for válido, o ESP32 central aciona os relés que destravam as fechaduras eletrônicas, permitindo a abertura da gaveta correspondente. Além disso, por meio do IP do ESP32, é possível acessar uma página web que contém campos para parametrização, monitoramento dos sensores, gravação de firmware à distância (OTA) e abertura das gavetas. Essa página é acessível mesmo na ausência de conexão Wi-Fi após a detecção de um QR Code com um comando específico, fazendo o ESP32 gerar um hotspot. É importante notar que o acesso a essa página requer uma senha.

O sistema também registra o tempo de abertura e fechamento de cada gaveta, atualizando o banco de dados por meio do método "PUT" ou salvando essas informações no cartão SD do ESP32-CAM para que possam ser atualizadas quando a conexão Wi-Fi for restabelecida. Sempre que houver alteração nos dados dos clientes, essas informações são salvas na EEPROM do ESP32, garantindo que, em caso de reinício do sistema, os dados permaneçam disponíveis até que o produto seja retirado.

Neste projeto, utilizei EEPROM, protocolos de comunicação SPI (cartão SD ESP32CAM), I2C (Display), ESPNOW e HTTP (API), além de programação em HTML para desenvolver as páginas web de parametrização. Também implementei OTA (Over The Air) e projetei o circuito eletrônico em protoplaca.

![image](https://github.com/user-attachments/assets/376c87d5-02d1-49ef-b004-cbfe3c4e4abf)

