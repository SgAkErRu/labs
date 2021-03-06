import path = require('path');
import fs = require('fs');
import http = require('http');
import https = require('https');
import dotenv = require('dotenv');
import os = require('os');

// Express
import { ExpressApp } from './ExpressApp';

// сокеты
import { SocketHandler } from './SocketHandler';

// для ввода в консоль
import readline = require('readline');

const DEFAULT_CONFIG_PATH = path.resolve(process.cwd(), 'config', 'server.default.conf');
const CUSTOM_CONFIG_PATH = path.resolve(process.cwd(), 'config', 'server.conf');

// загрузка значений из конфигурационного файла
function prepareConfig(): boolean
{
    let config_path = CUSTOM_CONFIG_PATH;

    if (!fs.existsSync(config_path) && fs.existsSync(DEFAULT_CONFIG_PATH))
    {
        config_path = DEFAULT_CONFIG_PATH;
    }
    else if (!fs.existsSync(config_path) && !fs.existsSync(DEFAULT_CONFIG_PATH))
    {
        return false;
    }

    dotenv.config({ path: config_path });
    return true;
}

// инициализация приложения
function initApplication()
{
    // загрузка значений из конфигурационного файла
    // выполняем первым делом, так как в конфиге написано название файла для лога
    const configLoadSuccess = prepareConfig();

    // считываем название и версию программы
    // именно таким способом, чтобы не были нужны переменные окружения npm
    const packageJson: unknown = JSON.parse(fs.readFileSync(path.resolve(process.cwd(), "package.json"), "utf8"));
    const { name, version } = packageJson as { name: string; version: string; };

    // выводим шапку
    process.title = `${name}-${version}`;
    console.log(`Version: ${version}`);

    // если конфиг не удалось загрузить
    if (!configLoadSuccess)
        throw new Error(`Отсутствует конфигурационный файл: ${DEFAULT_CONFIG_PATH}`);

}

// главная функция
async function main()
{
    try
    {
        // инициализируем приложение
        initApplication();

        const Express = new ExpressApp();

        const httpServer: http.Server = http.createServer(Express.app);
        const httpPort = process.env.HTTP_PORT;

        httpServer.listen(httpPort, () =>
        {
            console.log(`Http server running on port: ${httpPort!}`);
        });

        // настройки https-сервера (сертификаты)
        const httpsOptions: https.ServerOptions = {
            key: fs.readFileSync(path.resolve(process.cwd(), 'config', 'ssl', process.env.SSL_PRIVATE_KEY!), 'utf8'),
            cert: fs.readFileSync(path.resolve(process.cwd(), 'config', 'ssl', process.env.SSL_PUBLIC_CERT!), 'utf8')
        };

        const server: https.Server = https.createServer(httpsOptions, Express.app);
        const port = process.env.HTTPS_PORT;

        server.listen(port, () =>
        {
            console.log(`Https server running on port: ${port!}`);
        });

        const socketHandler = new SocketHandler(server);
    }
    catch (err)
    {
        console.error(`[ОШИБКА] ${(err as Error).message}`);
    }

    // для ввода в консоль сервера
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    rl.on('line', (input_str) =>
    {
        console.log(input_str);
    });
    rl.on('SIGINT', () =>
    {
        process.exit();
    });
}

// вызов главной функции
main().catch((reason) => console.error(reason));