#include <pagesWeb.h>

// Páginas HTML utilizadas
String verifica = R"(<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Configs</title>
    <meta charset='UTF-8'>
    <style>
        body {
            background-color: black;
            color: white;
            text-align: center;
            font-family: Arial, sans-serif;
            font-size: 18px; /* Aumenta o tamanho da fonte global */
        }
        h1 {
            margin-top: 40px; /* Aumenta a margem superior */
            font-size: 36px; /* Aumenta o tamanho da fonte do título */
        }
        form {
            margin-top: 150px; /* Aumenta a margem superior do formulário */
            display: inline-block;
            text-align: center;
        }
        label, input {
            display: block;
            margin: 20px auto; /* Aumenta a margem */
            font-size: 20px; /* Aumenta o tamanho da fonte */
        }
        input[type='submit'], input[type='file'], input[type='text'], input[type='password'] {
            background-color: #237B72; /* Cor azul ciano */
            color: white;
            border: none;
            padding: 20px; /* Aumenta o padding */
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 20px; /* Aumenta o tamanho da fonte */
            margin: 30px auto; /* Aumenta a margem */
            cursor: pointer;
            border-radius: 4px;
            transition: transform 0.3s; /* Adiciona transição suave */
        }
        input[type='file'], input[type='text'], input[type='password'] {
            padding: 15px; /* Aumenta o padding */
            margin: 20px auto; /* Aumenta a margem */
            border-radius: 4px;
            border: 1px solid #ccc;
            width: 400px; /* Aumenta a largura */
            max-width: 100%;
            box-sizing: border-box;
        }
        input[type='submit']:hover, input[type='file']:hover, input[type='text']:hover, input[type='password']:hover {
            transform: scale(1.05); /* Aumenta o tamanho das caixas ao passar o mouse */
        }
        a {
            color: white;
            text-decoration: none;
        }
        #siteName {
            position: absolute;
            bottom: 10px;
            right: 10px;
            color: white;
        }
    </style>
</head>
<body>
    <h1>ESP32 Configs</h1>
    <form method='POST' action='/avalia' enctype='multipart/form-data'>
        <label style='color:white;'>Autorização:</label>
        <input type='text' name='autorizacao'>
        <input type='submit' value='Ok'>
    </form>
    <div id='siteName'>
        <p style='color:white;'>Para mais informações, acesse: <a href='http://www.sherlock.com' style='text-decoration:underline;'>www.sherlock.com</a></p>
    </div>
</body>
</html>)";

String serverIndex = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Parâmetros ESP32</title>
    <meta charset='UTF-8'>
    <style>
        body {
            background-color: black;
            color: white;
            text-align: center;
            font-family: Arial, sans-serif;
        }
        h1 {
            margin-top: 20px;
            font-size: 36px;
            text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
        }
        form {
            margin-top: 100px;
            display: inline-block;
            text-align: center;
        }
        label {
            display: inline-block;
            vertical-align: middle;
            margin-bottom: 5px;
            font-size: 20px;
            text-align: left;
            width: 120px;
            margin-right: 10px;
            line-height: 2.5em;
        }
        input[type='submit'], button {
            background-color: #237B72;
            color: white;
            border: none;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            cursor: pointer;
            border-radius: 4px;
            transition: transform 0.3s, background-color 0.3s;
        }
        input[type='submit']:hover, button:hover {
            transform: scale(1.05);
            background-color: #1a6c63;
        }
        input[type='file'], input[type='text'], input[type='password'] {
            padding: 10px;
            border-radius: 4px;
            border: 1px solid #ccc;
            width: 300px;
            max-width: 100%;
            box-sizing: border-box;
        }
        input[type='text']:focus, input[type='password']:focus, input[type='file']:focus {
            border-color: #237B72;
            outline: none;
            box-shadow: 0 0 5px rgba(35, 123, 114, 0.5);
        }
        .file-upload-container, .input-container {
            display: flex;
            align-items: center;
            margin-bottom: 20px;
            justify-content: center;
        }
        .file-upload-container {
            width: 100%;
            max-width: 500px;
            gap: 10px;
        }
        .input-container {
            flex-direction: row;
            gap: 10px;
            width: 100%;
            max-width: 500px;
        }
        .button-container {
            display: flex;
            justify-content: center; /* Centraliza todos os botões */
            gap: 10px; /* Espaçamento entre os botões */
            margin-top: 30px;
            flex-wrap: wrap; /* Permite que os botões se movam para a linha seguinte se necessário */
        }
        .wifi-parameters, .api-parameters {
            font-size: 20px;
            color: white;
        }
        .wifi-parameters {
            margin-top: 40px;
            margin-bottom: 10px;
        }
        .api-parameters {
            margin-top: 40px;
            margin-bottom: 10px;
        }
        a {
            color: white;
            text-decoration: none;
        }
        footer {
            background-color: #1a6c63;
            color: white;
            padding: 10px;
            text-align: center;
            margin-top: 50px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .footer-left {
            margin-left: 10px;
        }
        .footer-right {
            margin-right: 10px;
            text-align: right;
        }
    </style>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
</head>
<body>
    <h1>Parâmetros ESP32</h1>
    <form method='POST' action='/update' enctype='multipart/form-data'>
        <div class="file-upload-container">
            <label for='file' style='color:white;'>Firmware:</label>
            <input type='file' name='update' id='fileInput' style='display: none;' onchange="updateFileName()">
            <button type='button' onclick='document.getElementById("fileInput").click();'>Escolher Arquivo</button>
            <input type='submit' value='Atualizar'>
        </div>
        <div class="wifi-parameters">Parâmetros do Wi-Fi</div>
        <div class="input-container">
            <label for='ssid' style='color:white;'>SSID:</label>
            <input type='text' name='ssid' placeholder='Digite o SSID'>
        </div>
        <div class="input-container">
            <label for='password' style='color:white;'>Senha:</label>
            <input type='password' name='password' placeholder='Digite a senha'>
        </div>
        <div class="api-parameters">Parâmetros de conexão com API</div>
        <div class="input-container">
            <label for='serverIP' style='color:white;'>IP:</label>
            <input type='text' name='serverIP' placeholder='Digite o IP'>
        </div>
        <div class="input-container">
            <label for='newField2' style='color:white;'>Porta:</label>
            <input type='text' name='newField2' placeholder='Digite a Porta'>
        </div>
        <div class="button-container">
            <button type='button' id='saveParams'>Salvar Parâmetros</button>
            <button type='button' id='button1'><i class="fa fa-cabinet"></i> Abrir gaveta 1</button>
            <button type='button' id='button2'><i class="fa fa-cabinet"></i> Abrir gaveta 2</button>
            <button type='button' id='uploadSD'>Upload cartão SD</button>
        </div>
    </form>
    <div id="sensor-data" style="margin-top: 40px;">
        <h2>Leitura dos Sensores</h2>
        <p>Distância Sensor 1: <span id="distance1">Aguardando...</span> cm</p>
        <p>Distância Sensor 2: <span id="distance2">Aguardando...</span> cm</p>
    </div>  
    <footer>
        <div class="footer-left">
            <p>&copy; 2024 SherLock. Todos os direitos reservados.</p>
        </div>
        <div class="footer-right">
            <p>Para mais informações, acesse: <a href='http://www.sherlockk.com' style='text-decoration: underline;'>www.sherlock.com</a></p>
        </div>
    </footer>
    <script>
        function updateFileName() {
            var fileInput = document.getElementById('fileInput');
            var button = document.querySelector('.file-upload-container button');
            var fileName = fileInput.files.length > 0 ? fileInput.files[0].name : 'Escolher Arquivo';
            button.textContent = fileName;
        }

        function handleResponse(response) {
            response.text().then(text => {
                console.log(text);
                // Exibe uma mensagem de sucesso ou erro na página
                alert(text);
            }).catch(error => {
                console.error('Erro ao processar a resposta:', error);
                alert('Ocorreu um erro ao processar a resposta.');
            });
        }

        document.getElementById('saveParams').addEventListener('click', function() {
            fetch('/saveParams', { 
                method: 'POST', 
                body: new FormData(document.querySelector('form'))
            })
            .then(handleResponse)
            .catch(error => {
                console.error('Erro na solicitação:', error);
                alert('Ocorreu um erro na solicitação.');
            });
        });

        document.getElementById('button1').addEventListener('click', function() {
            fetch('/gaveta1', { method: 'POST' })
                .then(handleResponse)
                .catch(error => {
                    console.error('Erro na solicitação:', error);
                    alert('Ocorreu um erro na solicitação.');
                });
        });

        document.getElementById('button2').addEventListener('click', function() {
            fetch('/gaveta2', { method: 'POST' })
                .then(handleResponse)
                .catch(error => {
                    console.error('Erro na solicitação:', error);
                    alert('Ocorreu um erro na solicitação.');
                });
        });

        document.getElementById('uploadSD').addEventListener('click', function() {
            fetch('/updateImages', { method: 'POST' })
                .then(handleResponse)
                .catch(error => {
                    console.error('Erro na solicitação:', error);
                    alert('Ocorreu um erro na solicitação.');
                });
        });

        function updateSensorData() {
            fetch('/getSensorData')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('distance1').textContent = data.distance1;
                    document.getElementById('distance2').textContent = data.distance2;
                })
                .catch(error => console.error('Erro ao atualizar os dados dos sensores:', error));
        }

        // Atualiza os dados dos sensores a cada segundo
        setInterval(updateSensorData, 1000);
    </script>
</body>
</html>
)rawliteral";
String Resultado_Ok = "<!DOCTYPE html><html><head><title>ESP32 Configs</title><meta charset='UTF-8'><style>body{background-color:black;color:white;text-align:center;font-family:Arial,sans-serif;position:relative;height:100vh;display:flex;flex-direction:column;justify-content:center}h1{margin-top:20px}h2{margin:auto}#siteName{position:absolute;bottom:10px;right:10px}</style></head><body><h1>ESP32 Configs</h1><h2>Atualização bem sucedida!</h2><div id='siteName'><p style='color:white;'>Para mais informações, acesse: <a href='http://www.sherlockk.com' style='text-decoration:underline;'>www.sherlockk.com</a></p></div></body></html>";
String Resultado_Falha = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Configs</title>
    <meta charset='UTF-8'>
    <style>
        body {
            background-color: black;
            color: white;
            text-align: center;
            font-family: Arial, sans-serif;
            position: relative;
            height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: center;
        }

        h1 {
            margin-top: 20px;
        }

        #siteName {
            position: absolute;
            bottom: 10px;
            right: 10px;
        }

        #backButton {
            margin-top: 20px;
            padding: 10px 20px;
            background-color: #333;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }

        #backButton:hover {
            background-color: #555;
        }

        #failureMessage {
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <h2 id='failureMessage'>Falha durante a atualização. A versão anterior do firmware será recarregada.</h2>
    <form action='/serverIndex'>
        <input id='backButton' type='submit' value='Voltar à seleção de firmware'>
    </form>
    <div id='siteName'>
        <p style='color:white;'>Para mais informações, acesse: <a href='http://www.sherlockk.com' style='text-decoration:underline;'>www.sherlock.com</a></p>
    </div>
</body>
</html>
)";