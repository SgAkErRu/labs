#ifndef ERRORS_H
#define ERRORS_H

#include <QString>
#include <QSqlError>

namespace Errors
{
    enum ErrCode: int {
        not_null_violation = 23502,
        check_violation = 23514,
        duplicate_object = 42710,
        reserved_name = 42939,
        unique_violation = 23505,
        foreign_key_violation = 23503
    };

    static QString msg(QSqlError errorCode)
    {
        if (errorCode.type() == QSqlError::ConnectionError)
        {
            if (errorCode.text().indexOf("could not connect to server") != -1)
                return "Не удалось подключиться к базе данных!";
            if (errorCode.text().indexOf("no password supplied") != -1)
                return "Необходимо указать пароль для авторизации!";
            if (errorCode.text().indexOf("password authentication failed") != -1)
                return "Неправильный логин или пароль!";
            return "Непредвиденная ошибка, сообщите администратору следующее сообщение:\n" + errorCode.text();
        }
        else
        {
            const auto code = errorCode.nativeErrorCode().toInt();
            switch(code)
            {
                case ErrCode::not_null_violation:
                    return "Пожалуйста, заполните все обязательные поля!";
                case ErrCode::check_violation:
                {
                    if (errorCode.text().indexOf("менеджер_телефон_check") != -1)
                        return "Номер телефона должен состоять из 11 символов!";
                    if (errorCode.text().indexOf("пункт_выдачи_площадь_check") != -1)
                        return "Площадь помещения должна быть больше 0 кв. м.";
                    if (errorCode.text().indexOf("товар_гарантийный_срок_check") != -1)
                        return "Гарантийный срок должен быть не менее 1 месяца!";
                    if (errorCode.text().indexOf("товар_стоимость_check") != -1)
                        return "Нулевая или отрицательная стоимость недопустима!";
                    if (errorCode.text().indexOf("товар_на_складе_количество_check") != -1)
                        return "Количество товара не может быть отрицательным!";
                    if (errorCode.text().indexOf("Insert product with count 0") != -1)
                        return "Нельзя добавить товар на склад с количеством равным нулю!";
                    if (errorCode.text().indexOf("Product in stock count less than required") != -1)
                        return "Количество продуктов на складе меньше требуемого. Обновите каталог товаров и попробуйте заказать снова.";
                    break;
                }
                case ErrCode::duplicate_object:
                {
                    if (errorCode.text().indexOf("role") != -1)
                        return "Данный пользователь уже зарегестрирован!";

                    break;
                }
                case ErrCode::unique_violation:
                    return "Нарушение требования уникальности значения в столбце таблицы!";
                case ErrCode::reserved_name:
                    return "Указанный логин занят другим пользователем!";
                case ErrCode::foreign_key_violation:
                    return "Данную запись невозможно удалить, так как она используется в других таблицах!";
            }
            return "Непредвиденная ошибка, сообщите администратору следующий код: " + QString::number(code);
        }
    }
}

#endif // ERRORS_H

