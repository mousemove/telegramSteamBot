Многопоточный бот для телеграма написанный на C++
Бот уведомляет о цене и скидках на товары в Steam. Если товар получает скидку, пользователь раз в день будет о ней проинформирован. Если на товар скидки на данный момент нет, то подписка сохраняется, и когда он получит её - пользователю придет ответ.

Бот может взаимодействовать с телеграмом через webhook(для этого вам нужен сертификат, установка webHook и открытый 8443 или 443 порт на компьютере/сервере). Так же нужно иметь доступ к api.telegram.org, в России он как известно блокируется

Написано на C++  с использованием с++11,sqlite3(база данных),curl,ssl и библиотеки https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/ для работы с json(не входит в репозиторий). При линковке указывайте -lpthread -lsqlite3 -lcurl -lcrypto -lss. Или просто воспользуйтесь makefile


@steamsalesrussia_bot - адрес бота в телеграме

Вложен так же пример простого бота, который отвечает на сообщения пользователя. Для его использования подключайте AbstractBot а не SteamBot


