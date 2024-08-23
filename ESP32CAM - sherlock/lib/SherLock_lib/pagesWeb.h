#ifndef __pagesWeb__
#define __pagesWeb__

#include <Arduino.h>

// Páginas HTML utilizadas no procedimento OTA
String verifica = R"(<!DOCTYPE html>
<html>
<head>
    <title>ESP32CAM Configs</title>
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
    <h1>ESP32CAM Configs</h1>
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

String Resultado_Ok = "<!DOCTYPE html><html><head><title>ESP32CAM Configs</title><meta charset='UTF-8'><style>body{background-color:black;color:white;text-align:center;font-family:Arial,sans-serif;position:relative;height:100vh;display:flex;flex-direction:column;justify-content:center}h1{margin-top:20px}h2{margin:auto}#siteName{position:absolute;bottom:10px;right:10px}</style></head><body><h1>ESP32CAM Configs</h1><h2>Atualização bem sucedida!</h2><div id='siteName'><p style='color:white;'>Para mais informações, acesse: <a href='http://www.sherlock.com' style='text-decoration:underline;'>www.sherlock.com</a></p></div></body></html>";
String Resultado_Falha = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32CAM Configs</title>
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
        <p style='color:white;'>Para mais informações, acesse: <a href='http://www.sherlock.com' style='text-decoration:underline;'>www.sherlock.com</a></p>
    </div>
</body>
</html>
)";

String serverIndex = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Parâmetros ESP32CAM</title>
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
            justify-content: center;
            gap: 20px;
            margin-top: 30px;
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
        #videoContainer {
            display: none;
            margin-top: 20px;
        }
        #videoStream {
            width: 100%;
            max-width: 640px;
            border: 2px solid white;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
        }
    </style>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
</head>
<body>
    <h1>Parâmetros ESP32CAM</h1>
    <form method='POST' action='/update' enctype='multipart/form-data'>
        <div class="file-upload-container">
            <label for='file' style='color:white;'>Firmware:</label>
            <input type='file' name='update' id='fileInput' style='display: none;' onchange="updateFileName()">
            <button type='button' onclick='document.getElementById("fileInput").click();'>Escolher Arquivo</button>
            <input type='submit' value='Atualizar'>
        </div>
        <div class="button-container">
            <button type='button' id='startStream'>Iniciar transmissão</button>
            <button type='button' id='button1'><i class="fa fa-cabinet"></i>Salvar fotos</button>
            <button type='button' id='button2'><i class="fa fa-cabinet"></i>Encerrar transmissão</button>
        </div>
    </form>

    <div id="videoContainer">
        <video id="videoStream" autoplay></video>
    </div>

    <footer>
        <div class="footer-left">
            <p>&copy; 2024 SherLock. Todos os direitos reservados.</p>
        </div>
        <div class="footer-right">
            <p>Para mais informações, acesse: <a href='http://www.sherlock.com' style='text-decoration: underline;'>www.sherlock.com</a></p>
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
                alert(text);
            }).catch(error => {
                console.error('Erro ao processar a resposta:', error);
                alert('Ocorreu um erro ao processar a resposta.');
            });
        }

        document.getElementById('startStream').addEventListener('click', function() {
            fetch('/InitTransmition', { method: 'POST' })
                .then(() => {
                    document.getElementById('videoContainer').style.display = 'block';
                    document.getElementById('videoStream').src = '/stream';
                })
                .catch(error => {
                    console.error('Erro ao iniciar a transmissão:', error);
                    alert('Ocorreu um erro ao iniciar a transmissão.');
                });
        });

        document.getElementById('button1').addEventListener('click', function() {
            fetch('/uploadFotos', { method: 'POST' })
                .then(handleResponse)
                .catch(error => {
                    console.error('Erro na solicitação:', error);
                    alert('Ocorreu um erro na solicitação.');
                });
        });

        document.getElementById('button2').addEventListener('click', function() {
                fetch('/EndTransmition', { method: 'POST' })
                .then(() => {
                    document.getElementById('videoContainer').style.display = 'none';
                    document.getElementById('videoStream').src = '';
                })
                .catch(error => {
                    console.error('Erro ao encerrar a transmissão:', error);
                    alert('Ocorreu um erro ao encerrar a transmissão.');
                });
        });
    </script>
</body>
</html>
)rawliteral";

#endif