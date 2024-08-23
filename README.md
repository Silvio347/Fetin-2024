# Fetin1
Meu projeto da Fetin 2024. 

Apresentação do Projeto SHERLOCK:

O Projeto SHERLOCK visa automatizar o processo de retirada de produtos adquiridos em e-commerce, oferecendo uma solução prática e segura para os clientes. O conceito é simples: o cliente final recebe um QR Code, que é utilizado para abrir uma gaveta onde seu produto está armazenado. Essas gavetas fazem parte de armários inteligentes distribuídos em locais estratégicos, como faculdades, mercados e outros locais que oferecem segurança. Dessa forma, pessoas que têm dificuldade em receber suas encomendas em casa podem optar por retirar seus produtos nesses armários, onde os entregadores utilizam um QR Code específico para depositar as mercadorias nas gavetas designadas.

Detalhamento Técnico:
De forma simplificada, o sistema funciona da seguinte maneira: o QR Code é escaneado por uma câmera ESP32-CAM, que envia os dados para um outro dispositivo ESP32. Este, por sua vez, processa a informação e verifica sua validade com base em um banco de dados. Se o código for válido, o ESP32 central aciona os relés que destravam as fechaduras eletrônicas, permitindo que a gaveta correspondente seja aberta.
