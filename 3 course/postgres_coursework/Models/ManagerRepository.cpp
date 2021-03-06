#include "ManagerRepository.h"
#include "StringTools.h"

using namespace StringTools;

QString ManagerRepository::getSelectQuery() const
{
    const Tokens args = {"SELECT id_менеджер",
                         ", фамилия as", quote("Фамилия"),
                         ", имя as", quote("Имя"),
                         ", отчество as", quote("Отчество"),
                         ", телефон as", quote("Телефон"),
                         ", email as", quote("Электронная почта"),
                         "FROM менеджер"};
    return vecToStr(args);
}

bool ManagerRepository::create(const ManagerModel &data)
{
    db.prepare("SELECT add_manager(:lastname, :firstname, :otchestvo, :telephone, :email, :password)");
    db.bindValue(":lastname", convertIfNull(data.lastname));
    db.bindValue(":firstname", convertIfNull(data.firstname));
    db.bindValue(":otchestvo", convertIfNull(data.otchestvo));
    db.bindValue(":telephone", convertIfNull(data.telephone));
    db.bindValue(":email", convertIfNull(data.email));
    db.bindValue(":password", data.password);

    bool result = db.exec();
    if (result) this->read();
    return result;
}

bool ManagerRepository::read() const
{
    return db.execWithDisplay(getSelectQuery() + " ORDER BY id_менеджер");
}

bool ManagerRepository::update(const ManagerModel &data)
{
    bool result = false;
    if (data.id)
    {
        db.prepare("SELECT update_manager(:id, :lastname, :firstname, :otchestvo, :telephone, :email, :password)");
        db.bindValue(":id", data.id);
        db.bindValue(":lastname", convertIfNull(data.lastname));
        db.bindValue(":firstname", convertIfNull(data.firstname));
        db.bindValue(":otchestvo", convertIfNull(data.otchestvo));
        db.bindValue(":telephone", convertIfNull(data.telephone));
        db.bindValue(":email", convertIfNull(data.email));
        db.bindValue(":password", data.password);

        result = db.exec();
        if (result) this->read();
    }
    return result;
}

bool ManagerRepository::remove(int id)
{
    bool result = false;
    if (id)
    {
        db.prepare("SELECT delete_manager(:id)");
        db.bindValue(":id", id);

        result = db.exec();
        if (result) this->read();
    }
    return result;
}

void ManagerRepository::search(const ManagerModel &data)
{
    Tokens searchOptions;
    QString query = getSelectQuery() + " WHERE ";
    if (!data.lastname.isEmpty())
        searchOptions.emplace_back("фамилия ILIKE :lastname");

    if (!data.firstname.isEmpty())
        searchOptions.emplace_back("имя ILIKE :firstname");

    if (!data.otchestvo.isEmpty())
        searchOptions.emplace_back("отчество ILIKE :otchestvo");

    if (!data.telephone.isEmpty())
        searchOptions.emplace_back("телефон LIKE :telephone");

    if (!data.email.isEmpty())
        searchOptions.emplace_back("email ILIKE :email");

    const size_t N = searchOptions.size();
    if (!searchOptions.empty())
    {
        for (size_t i = 0; i < N-1; ++i)
        {
            query += searchOptions[i] + " AND ";
        }
        query += searchOptions[N-1];
        db.prepare(query);

        db.bindValue(":lastname", "%" + data.lastname + "%");
        db.bindValue(":firstname", "%" + data.firstname + "%");
        db.bindValue(":otchestvo", "%" + data.otchestvo + "%");
        db.bindValue(":telephone", "%" + data.telephone + "%");
        db.bindValue(":email", "%" + data.email + "%");

        db.execWithDisplay();
    }
    else
    {
        this->read();
    }
}
