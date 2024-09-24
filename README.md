# Fetin
Meu projeto da Fetin 2024. 

--------Apresentação do Projeto SHERLOCK:

O Projeto SHERLOCK visa automatizar o processo de retirada de produtos adquiridos em e-commerce, oferecendo uma solução prática e segura para os clientes. O conceito é simples: o cliente final recebe um QR Code, que é utilizado para abrir uma gaveta onde seu produto está armazenado. Essas gavetas fazem parte de armários inteligentes distribuídos em locais estratégicos, como faculdades, mercados e outros locais que oferecem segurança. Dessa forma, pessoas que têm dificuldade em receber suas encomendas em casa podem optar por retirar seus produtos nesses armários, onde os entregadores utilizam um QR Code diferente para depositar as mercadorias nas gavetas designadas.

--------Detalhamento Técnico:

De forma simplificada, o sistema funciona da seguinte maneira: o QR Code é escaneado por uma câmera ESP32-CAM, que envia os dados para um outro dispositivo ESP32. Este, por sua vez, processa a informação e verifica sua validade com base em um banco de dados. Se o código for válido, o ESP32 central aciona os relés que destravam as fechaduras eletrônicas, permitindo que a gaveta correspondente seja aberta. Além disso, através do IP do ESP32 é possível acessar uma página web com campos para parametrização, monitoramento dos sensores e abertura das gavetas. Essa página é acessível mesmo na ausência de conexão Wi-Fi após a detecção de um QR Code que tem um comando específico, fazendo o ESP32 gerar um hotspot. Nota: Para acessar essa página é necessário uma senha.
Também contamos com fins de curso em cada gaveta para salvar o tempo de abertura/fechamento de cada gaveta, atualizando o banco de dados através do método "PUT" ou salvando essas informações no cartão SD do ESP32CAM para atualizar quando voltar a conexão WiFi. Sempre que houver alteração nos dados dos clientes quando for obter os dados do banco de dados, salvamos na EEPROM do ESP32 para se caso o sistema for reiniciado, ter salvo até o produto ser retirado.

![image](https://github.com/user-attachments/assets/376c87d5-02d1-49ef-b004-cbfe3c4e4abf)

