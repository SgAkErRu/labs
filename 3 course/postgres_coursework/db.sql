-- Производитель
CREATE TABLE производитель
(
	id_производитель integer GENERATED BY DEFAULT AS IDENTITY, 
	название text NOT NULL, 
	страна text NOT NULL,
	email text NOT NULL,
	PRIMARY KEY (id_производитель)
);

-- Категория
CREATE TABLE категория
(
	id_категория integer GENERATED BY DEFAULT AS IDENTITY, 
	название text NOT NULL,
	PRIMARY KEY (id_категория)
);

-- Товар
CREATE TABLE товар
(
	id_товар integer GENERATED BY DEFAULT AS IDENTITY, 
	id_производитель integer NOT NULL,
	id_категория integer NOT NULL,
	наименование text NOT NULL, 
	стоимость real NOT NULL CHECK (стоимость > 0), 
	гарантийный_срок smallint NOT NULL CHECK (гарантийный_срок > 0),
	характеристики jsonb,
	PRIMARY KEY (id_товар),
	CONSTRAINT производитель_exists FOREIGN KEY (id_производитель)
        REFERENCES производитель (id_производитель)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT категория_exists FOREIGN KEY (id_категория)
        REFERENCES категория (id_категория)
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

-- Поставщик
CREATE TABLE поставщик
(
	id_поставщик integer GENERATED BY DEFAULT AS IDENTITY, 
	название text NOT NULL, 
	инн varchar(12) NOT NULL CHECK (LENGTH(инн) = 12), 
	город text NOT NULL,
	телефон varchar(11) NOT NULL CHECK (LENGTH(телефон) = 11), 
	email text NOT NULL,
	PRIMARY KEY (id_поставщик)
);

-- Склад
CREATE TABLE склад
(
	id_склад integer GENERATED BY DEFAULT AS IDENTITY, 
	название text NOT NULL,
	адрес text NOT NULL,	
	площадь real NOT NULL CHECK (площадь > 0),
	PRIMARY KEY (id_склад)
);

-- Товар на складе
CREATE TABLE товар_на_складе
(
	id_товар_на_складе integer GENERATED BY DEFAULT AS IDENTITY, 
	id_товар integer NOT NULL,
	id_поставщик integer NOT NULL,
	id_склад integer NOT NULL,
	дата_поступления date NOT NULL,
	дата_изготовления date NOT NULL,
	количество smallint CHECK (количество >= 0),
	PRIMARY KEY (id_товар_на_складе),
	CONSTRAINT товар_exists FOREIGN KEY (id_товар)
        REFERENCES товар (id_товар)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT поставщик_exists FOREIGN KEY (id_поставщик)
        REFERENCES поставщик (id_поставщик)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT склад_exists FOREIGN KEY (id_склад)
        REFERENCES склад (id_склад)
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

-- Пункт выдачи
CREATE TABLE пункт_выдачи
(
	id_пункт_выдачи integer GENERATED BY DEFAULT AS IDENTITY, 
	название text NOT NULL,
	адрес text NOT NULL,	
	площадь real NOT NULL CHECK (площадь > 0),
	доп_функции text[], -- оплата картой, наличными, возможность примерки
	PRIMARY KEY (id_пункт_выдачи)
);

-- Клиент
CREATE TABLE клиент
(
	id_клиент integer GENERATED BY DEFAULT AS IDENTITY, 
	фамилия text NOT NULL, 
	имя text, 
	отчество text, 
	телефон varchar(11) NOT NULL CHECK (LENGTH(телефон) = 11), 
	email text,
	PRIMARY KEY (id_клиент)
);

-- менеджер
CREATE TABLE менеджер
(
	id_менеджер integer GENERATED BY DEFAULT AS IDENTITY, 
	фамилия text NOT NULL, 
	имя text NOT NULL, 
	отчество text, 
	телефон varchar(11) NOT NULL CHECK (LENGTH(телефон) = 11), 
	email text NOT NULL,
	PRIMARY KEY (id_менеджер)
);

-- Статус
CREATE TABLE статус
(
	id_статус integer GENERATED BY DEFAULT AS IDENTITY, 
	название text NOT NULL, -- открыт, завершен, отменён
	PRIMARY KEY (id_статус)
);

-- Заказ
CREATE TABLE заказ
(
	id_заказ integer GENERATED BY DEFAULT AS IDENTITY, 
	id_клиент integer NOT NULL,
	id_статус integer NOT NULL,
	id_пункт_выдачи integer NOT NULL,
	id_менеджер integer,
	дата_оформления date NOT NULL,
	дата_выдачи date,
	общая_сумма real NOT NULL CHECK (общая_сумма > 0),
	PRIMARY KEY (id_заказ),
	CONSTRAINT клиент_exists FOREIGN KEY (id_клиент)
        REFERENCES клиент (id_клиент)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT статус_exists FOREIGN KEY (id_статус)
        REFERENCES статус (id_статус)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT пункт_выдачи_exists FOREIGN KEY (id_пункт_выдачи)
        REFERENCES пункт_выдачи (id_пункт_выдачи)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT менеджер_exists FOREIGN KEY (id_менеджер)
        REFERENCES менеджер (id_менеджер)
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

-- Позиция в заказе
CREATE TABLE позиция_заказа
(
	id_позиция_заказа integer GENERATED BY DEFAULT AS IDENTITY, 
	id_заказ integer NOT NULL,
	id_товар_на_складе integer NOT NULL,
	количество smallint CHECK (количество > 0),
	PRIMARY KEY (id_позиция_заказа),
	CONSTRAINT заказ_exists FOREIGN KEY (id_заказ)
        REFERENCES заказ (id_заказ)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
	CONSTRAINT товар_на_складе_exists FOREIGN KEY (id_товар_на_складе)
        REFERENCES товар_на_складе (id_товар_на_складе)
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

-- Создание ролей
CREATE USER operator PASSWORD 'oper123';
GRANT SELECT ON категория, менеджер, поставщик, производитель, пункт_выдачи, склад, товар_на_складе, товар TO operator;

CREATE GROUP managers;
GRANT SELECT ON заказ TO managers;

CREATE USER buyer PASSWORD 'buyer';

-- Функция добавления нового менеджера
CREATE OR REPLACE PROCEDURE add_manager
(IN surname text, IN name text, IN otchestvo text, IN tel text, IN email text, IN passw text) 
SECURITY DEFINER
AS $BODY$
BEGIN	
	INSERT INTO менеджер (фамилия, имя, отчество, телефон, email) 
		VALUES (surname, name, otchestvo, tel, email);
		
	EXECUTE FORMAT('CREATE USER %I PASSWORD ''%I'' IN ROLE managers', email, passw);
END;
$BODY$
LANGUAGE plpgsql 

GRANT EXECUTE ON PROCEDURE add_manager(text, text, text, text, text, text) TO operator;

CALL add_manager('Иванов', 'Иван', 'Иванович', '79999999979', 'test@mail.ru', 'test1');
CALL add_manager('Петров', 'Петр', 'Петрович', '79089195979', 'petr1@yandex.ru', 'petya');
CALL add_manager('Алексеев', 'Алексей', 'Алексеевич', '79889225979', 'alexey88@mail.ru', 'leha1');